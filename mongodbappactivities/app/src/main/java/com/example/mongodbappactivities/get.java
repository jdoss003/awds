package com.example.mongodbappactivities;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import android.app.ProgressDialog;
import android.os.AsyncTask;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v4.widget.CircularProgressDrawable;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Menu;
import android.widget.ProgressBar;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.support.v7.widget.Toolbar;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.net.HttpURLConnection;
import java.net.URL;
import java.nio.Buffer;

public class get extends AppCompatActivity {

    private TextView mResult;
    private Toolbar toolbar;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_get);
        toolbar =  (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        mResult = (TextView) findViewById(R.id.tv_result);

        //EXECUTING GET REQUEST
        //new GetDataTask().execute("http:192.168.1.149:1000/api/status"); //home
        //new GetDataTask().execute("http:192.168.43.51:1000/api/status"); //hotspot
        new GetDataTask().execute("http:192.168.43.51:1000/packages"); //hotspot Dereks api
    }

    class GetDataTask extends AsyncTask<String, Void, String> {
        ProgressDialog progressDialog;

        @Override
        protected void onPreExecute(){

            super.onPreExecute();
            progressDialog = new ProgressDialog(get.this);
            progressDialog.setMessage("Loading Data");
            progressDialog.show();
        }

        @Override
        protected String doInBackground(String ... params){

            StringBuilder result = new StringBuilder();

            //Initializing and configuring request and connecting to the server
            try{
                URL url = new URL(params[0]);
                HttpURLConnection urlConnection = (HttpURLConnection) url.openConnection();
                urlConnection.setReadTimeout(10000 /*milliseconds */);
                urlConnection.setConnectTimeout(10000 /*milliseconds */);
                urlConnection.setRequestMethod("GET");
                urlConnection.setRequestProperty("Content-Type", "application/json");//set header
                urlConnection.connect();

                //Read in data from server
                InputStream inputStream = urlConnection.getInputStream();
                BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(inputStream));
                String line;
                while((line = bufferedReader.readLine()) != null){
                    result.append(line).append("\n");
                }
            }
            catch (IOException ex)
            {
                return  "Network Errorrrrrr!";
            }
            return result.toString(); //returns json object(docs)
        }

        @Override
        protected void onPostExecute(String result) {
            super.onPostExecute(result);

            //setting data response to textview
            mResult.setText(result);

            //canceling progress dialog
            if(progressDialog != null){
                progressDialog.dismiss();
            }
        }
    }
}
