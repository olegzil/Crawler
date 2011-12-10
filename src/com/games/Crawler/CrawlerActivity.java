/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.games.Crawler;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;


public class CrawlerActivity extends Activity {

	CrawlerView mView;
    private String TAG="CrawlerActivity @@@@@@@@@@ ";

    public CharSequence readAsset(String asset) 
    { 
        BufferedReader in = null; 
        StringBuilder buffer = null; 
        try 
        { 
            in = new BufferedReader(new InputStreamReader (getAssets().open(asset))); 
            String line; 
            buffer = new StringBuilder(); 
            while ((line = in.readLine()) != null) 
            { 
                buffer.append(line).append('\n'); 
            } 
            return buffer; 
        } 
        catch (IOException e) 
        { 
            return ""; 
        }
    } 
    
    @Override protected void onCreate(Bundle icicle) 
    {
        super.onCreate(icicle);
        CharSequence pixelShader = readAsset("PixelShader1.s");
        CharSequence vertexShader = readAsset("vertexShader1.vs");
        //TODO:Add a directory file with a list of shader names.  Read the file and 
        //populate the string arrays accordingly.
        String[] pixelShaders = new String[1];
        String[] vertexShaders = new String[1];
        Log.d(TAG,pixelShader.toString());				//TODO:debug
        pixelShaders[0] = pixelShader.toString();
        vertexShaders[0] = vertexShader.toString();
        mView = new CrawlerView(getApplication(), pixelShaders, vertexShaders);
	setContentView(mView);
    }

    @Override protected void onPause() {
        super.onPause();
        mView.onPause();
    }

    @Override protected void onResume() {
        super.onResume();
        mView.onResume();
    }
    
}
