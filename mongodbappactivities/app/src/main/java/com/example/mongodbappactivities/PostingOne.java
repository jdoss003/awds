
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

public class PostingOne extends AppCompatActivity {

    private TextView mResult;
    private Toolbar toolbar;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_get);
        toolbar =  (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        mResult = (TextView) findViewById(R.id.tv_result);

        //EXECUTING POST REQUESTs
        new PostDataTask().execute("http:192.168.1.149:1000/api/status"); //home
        //new PostDataTask().execute("http:169.235.178.211:1000/api/status");//school
        //new PostDataTask().execute("http:192.168.43.51:1000/api/status"); //hotspot
    }

    class PostDataTask extends AsyncTask<String, Void, String> {
        ProgressDialog progressDialog;
        @Override
        protected void onPreExecute() {
            super.onPreExecute();
            progressDialog = new ProgressDialog(PostingOne.this);
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

                dataToSend.put("firstname", "Red");
                dataToSend.put("lastname", "Apple");
                dataToSend.put("packnum", 2019);
                dataToSend.put("storagelocation", "92507,92507");
                dataToSend.put("deliverylocation", "92507,92507");

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

