package com.example.mongodbappactivities;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

public class PostingCustom extends AppCompatActivity {


    Button btn;
    EditText editText;
    String string;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_posting_custom);

        btn = findViewById(R.id.custom_btn);
        editText = findViewById(R.id.edittext);

        btn.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                Intent i = new Intent(PostingCustom.this, PostingCustom2.class);
                string = editText.getText().toString();
                i.putExtra("PACKAGE_ID", string);

                editText = findViewById(R.id.edittext2);
                string = editText.getText().toString();
                i.putExtra("DATE_ENTERED", string);

                editText = findViewById(R.id.edittext3);
                string = editText.getText().toString();
                i.putExtra("DETAILS", string);

                editText = findViewById(R.id.edittext4);
                string = editText.getText().toString();
                i.putExtra("GCODE", string);


                startActivity(i);
                finish();
            }

        });
    }
}
