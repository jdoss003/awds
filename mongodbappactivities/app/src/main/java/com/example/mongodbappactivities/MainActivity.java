package com.example.mongodbappactivities;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

public class MainActivity extends AppCompatActivity {
    private Button button1, button2, button3;  //button1 = get, 2 = post, 3 = send

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        button1 = (Button) findViewById(R.id.button1);
        button1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                openActivityGet();
            }
        });

        button2 = (Button) findViewById(R.id.button2);
        button2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                openActivityPost();
            }
        });

        button3 = (Button) findViewById(R.id.button3);
        button3.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                openActivitySend();
            }
        });
    }

    public void openActivityGet() {
        Intent intent = new Intent(this, get.class);
        startActivity(intent);
    }
    public void openActivityPost() {
        Intent intent = new Intent(this, post.class);
        startActivity(intent);
    }
    public void openActivitySend() {
        Intent intent = new Intent(this, send.class);
        startActivity(intent);
    }
}