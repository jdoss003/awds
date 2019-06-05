package com.example.mongodbappactivities;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;
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

public class PostingCustom2 extends AppCompatActivity {

    TextView tv,tv2, tv3, tv4;
    String str, str2, str3, str4;
    private TextView mResult;
    private Toolbar toolbar;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_posting_custom2);

        //PRINTING USER INPUT
        tv = findViewById(R.id.tvr);
        str = getIntent().getExtras().getString("PACKAGE_ID");
        tv.setText(str);

        tv2 = findViewById(R.id.tvr2);
        str2 = getIntent().getExtras().getString("DATE_ENTERED");
        tv2.setText(str2);

        tv3 = findViewById(R.id.tvr3);
        str3 = getIntent().getExtras().getString("DETAILS");
        tv3.setText(str3);

        tv4 = findViewById(R.id.tvr4);
        str4 = getIntent().getExtras().getString("GCODE");
        tv4.setText(str4);

        //POSTING USER INPUT TO DATABASE
        toolbar =  (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        mResult = (TextView) findViewById(R.id.tv_result);

        //EXECUTING POST REQUESTs
        //new PostDataTask().execute("http:192.168.1.149:1000/api/status"); //home
        //new PostDataTask().execute("http:192.168.43.51:1000/api/status"); //hotspot
        new PostDataTask().execute("http:192.168.43.51:1000/packages"); //hotspot Dereks api

    }
    class PostDataTask extends AsyncTask<String, Void, String> {
        ProgressDialog progressDialog;
        @Override
        protected void onPreExecute() {
            super.onPreExecute();
            progressDialog = new ProgressDialog(PostingCustom2.this);
            progressDialog.setMessage("Inserting Data . . .");
            progressDialog.show();
        }

        @Override
        protected String doInBackground(String ... params) {
            try {
                return postData(params[0]);
            } catch (IOException ex) {
                return "Network Error!";
            } catch (JSONException ex) {
                return "Invalid Data!";
            }
        }

        @Override
        protected void onPostExecute(String result) {
            super.onPostExecute(result);

            mResult.setText(result);

            if(progressDialog != null){
                progressDialog.dismiss();
            }
        }

        private String postData(String urlPath)throws IOException, JSONException {

            StringBuilder result = new StringBuilder();
            BufferedWriter bufferedWriter = null;
            BufferedReader bufferedReader = null;
            try {
                //creating data for POST to server
                JSONObject dataToSend = new JSONObject();

                dataToSend.put("PACKAGE_ID", str);
                dataToSend.put("DATE_ENTERED", str2);
                dataToSend.put("DETAILS", str3);
                dataToSend.put("GCODE", str4);

                //init and config request to connect to server db
                URL url = new URL(urlPath);
                HttpURLConnection urlConnection = (HttpURLConnection) url.openConnection();
                urlConnection.setReadTimeout(10000 /*milliseconds */);
                urlConnection.setConnectTimeout(10000 /*milliseconds */);
                urlConnection.setRequestMethod("POST");
                urlConnection.setDoOutput(true);//enables output of body data json files
                urlConnection.setRequestProperty("Content-Type", "application/json");//set header
                urlConnection.connect();

                //Write data to server
                OutputStream outputStream = urlConnection.getOutputStream();
                bufferedWriter = new BufferedWriter(new OutputStreamWriter(outputStream));
                bufferedWriter.write(dataToSend.toString());
                bufferedWriter.flush();

                //read data response from server
                InputStream inputStream = urlConnection.getInputStream();
                bufferedReader = new BufferedReader(new InputStreamReader(inputStream));
                String line;
                while ((line = bufferedReader.readLine()) != null) {
                    result.append(line).append("\n");
                }
            } finally {
                if (bufferedReader != null) {
                    bufferedReader.close();
                }
                if(bufferedWriter != null) {
                    bufferedWriter.close();
                }
            }
            return result.toString();
        }
    }

}
