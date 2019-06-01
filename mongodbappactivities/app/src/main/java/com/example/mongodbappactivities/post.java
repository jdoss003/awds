package com.example.mongodbappactivities;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class post extends AppCompatActivity {

    public static TextView resultTextView;

    Button package_one_btn, package_two_btn, package_three_btn;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_post);

        resultTextView = (TextView)findViewById(R.id.result_text);

        package_one_btn = (Button) findViewById(R.id.package_one_btn);
        package_one_btn.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View view){
                startActivity(new Intent(getApplicationContext(), PostingOne.class));
            }
        });

        package_two_btn = (Button) findViewById(R.id.package_two_btn);
        package_two_btn.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View view){
                startActivity(new Intent(getApplicationContext(), PostingTwo.class));
            }
        });

        package_three_btn = (Button) findViewById(R.id.package_three_btn);
        package_three_btn.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View view){
                startActivity(new Intent(getApplicationContext(), PostingThree.class));
            }
        });
    }
}