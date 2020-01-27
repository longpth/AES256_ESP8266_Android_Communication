package com.p4f.aes256_android;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

import android.annotation.SuppressLint;
import android.os.AsyncTask;
import android.os.Build;
import android.os.SystemClock;
import android.util.Log;

public class UDPSocket {
    private AsyncTask<Void, Void, Void> async_client;
    private String receivedMessage = "NONE:NONE";

    private AsyncTask<Void, Void, Void> async_server;
    private boolean serverRunning = true;
    private int myPort = 6969;
    boolean responseReady = false;
    protected final Object lockResult = new Object();

    public CallBack callBack = null;

    UDPSocket(int port){
        myPort = port;
    }

    @SuppressLint("NewApi")
    public void sendMessage(final InetAddress addr,final int port,final String Message) {
        async_client = new AsyncTask<Void, Void, Void>() {
            @Override
            protected Void doInBackground(Void... params) {
                DatagramSocket ds = null;

                try {
                    ds = new DatagramSocket();
                    DatagramPacket dp;
                    dp = new DatagramPacket(Message.getBytes(), Message.length(), addr, port);
                    ds.setBroadcast(true);
                    ds.send(dp);
                } catch (Exception e) {
                    e.printStackTrace();
                } finally {
                    if (ds != null) {
                        ds.close();
                    }
                }
                return null;
            }

            protected void onPostExecute(Void result) {
                super.onPostExecute(result);
            }
        };

        if (Build.VERSION.SDK_INT >= 11)
            async_client.executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR);
        else async_client.execute();
    }

    @SuppressLint("NewApi")
    public void runUdpServer()
    {
        async_server = new AsyncTask<Void, Void, Void>()
        {
            @Override
            protected Void doInBackground(Void... params)
            {
                byte[] lMsg = new byte[4096];
                DatagramPacket dp = new DatagramPacket(lMsg, lMsg.length);
                DatagramSocket ds = null;

                try
                {
                    ds = new DatagramSocket(myPort);

                    while(serverRunning)
                    {
                        ds.receive(dp);

//                        Log.d("Received from lock", ": " +  new String(lMsg, 0, dp.getLength()));
                        synchronized(lockResult) {
                            receivedMessage =  new String(lMsg, 0, dp.getLength());
                            if(callBack != null){
                                callBack.processData(receivedMessage);
                            }
                            responseReady = true;
                            lockResult.notifyAll();
                        }
//                        Intent i = new Intent();
//                        i.setAction("Message receive");
//                        i.putExtra(Main.MESSAGE_STRING, new String(lMsg, 0, dp.getLength()));
//                        Main.MainContext.getApplicationContext().sendBroadcast(i);
                    }
                }
                catch (Exception e)
                {
                    e.printStackTrace();
                }
                finally
                {
                    if (ds != null)
                    {
                        ds.close();
                    }
                }

                return null;
            }
        };

        if (Build.VERSION.SDK_INT >= 11) async_server.executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR);
        else async_server.execute();
    }

    public interface CallBack{
        void processData(String recvMessage);
    }

    public String getResponse(){
        String ret;
        synchronized(lockResult) {
            while(!responseReady) {
                try {
                    lockResult.wait(1000);
                    responseReady = true;
                }catch (Exception e){
                    Log.d("Exception when waiting on receivedMessage", ": "+e.getMessage());
                }
            }
            ret = receivedMessage;
        }
        responseReady = false;

        return ret;
    }

    public boolean stopUDPServer()
    {
        serverRunning = false;
        SystemClock.sleep(10);
        return (async_server.getStatus() == AsyncTask.Status.RUNNING);
    }
}