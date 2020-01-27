package com.p4f.aes256_android;

import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

import java.net.InetAddress;

public class MainActivity extends AppCompatActivity {

    UDPSocket udpClient;

    EditText editTextESPAddr;
    EditText editTextESPPort;
    EditText editTextPass;
    EditText editMessageSend;
    EditText editMessageSendEncrypt;
    EditText editMessageRecv;
    EditText editMessageRecvEncrypt;

    UDPSocket.CallBack callBack = null;

    private String sendMessageUDP(String message, InetAddress addr, int port)
    {
        try {
            // encrypt message before sending
            AESCrypt aesCrypt = new AESCrypt(editTextPass.getText().toString());
            message = aesCrypt.encrypt(message);
            udpClient.sendMessage(addr, port, message);
        }catch(Exception e){
            Log.d("Exception", ": " + e.getMessage());
        }
        return message;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Button sendBtn = findViewById(R.id.sendBtn);

        editTextESPAddr = findViewById(R.id.esp8266Address);
        editTextESPPort = findViewById(R.id.esp8266Port);
        editTextPass = findViewById(R.id.passAES);
        editMessageSend = findViewById(R.id.sentMsg);
        editMessageSendEncrypt = findViewById(R.id.sentMsgEncrypt);
        editMessageRecvEncrypt = findViewById(R.id.receivedMsgEncrypt);
        editMessageRecv = findViewById(R.id.receivedMsg);

        sendBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                //test AES
                try {
                    InetAddress addr = InetAddress.getByName(editTextESPAddr.getText().toString());
                    int port = Integer.parseInt(editTextESPPort.getText().toString());
                    String encryptedMsg = sendMessageUDP(editMessageSend.getText().toString(), addr, port);
                    editMessageSendEncrypt.setText(encryptedMsg);
                }catch(Exception e){
                    Log.d("Exception ", ": " + e.getMessage());
                }
            }
        });

        callBack = new UDPSocket.CallBack() {
            @Override
            public void processData(final String recvMessage) {
                editMessageRecvEncrypt.post(new Runnable() {
                    public void run() {
                        editMessageRecvEncrypt.setText(recvMessage);
                        try{
                            AESCrypt aesCrypt = new AESCrypt(editTextPass.getText().toString());
                            String decryptedMsg = aesCrypt.decrypt(recvMessage);
                            editMessageRecv.setText(decryptedMsg);
                        }catch(Exception e){
                            Log.d("Exception: ", ""+e.getMessage());
                        }

                    }
                });
            }
        };

        udpClient = new UDPSocket(6969);
        udpClient.callBack = callBack;
        udpClient.runUdpServer();


    }
}
