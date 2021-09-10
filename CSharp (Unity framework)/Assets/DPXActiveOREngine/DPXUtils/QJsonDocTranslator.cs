using System;
using System.Text;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;
using DPXUtils;

namespace DPXUtils
{
    public class QJsonDocTranslator 
    {
        public byte[] JsonToBinaryData(string json)
        {
            return System.Text.Encoding.ASCII.GetBytes(json);
        }

        public string JsonFromBinaryData(byte[] binary_data, int size)
        {
            return System.Text.Encoding.UTF8.GetString(binary_data,0,size);
        }


        // Use this for test purposes
        public void ExampleCall()
        {
            string teststr = "{\n\"FirstName\": \"John\",\n\"LastName\": \"Doe\",\n\"Age\": 43}";

            Debug.Log("teststr: " + teststr);
            Debug.Log("teststr len: " + teststr.Length);

            byte[] res1 = JsonToBinaryData(teststr);

            Debug.Log("result JsonToBinaryData: " + System.Text.Encoding.UTF8.GetString(res1));
            Debug.Log("result JsonToBinaryData length: " + res1.Length);

            string res2 = JsonFromBinaryData(res1, res1.Length);

            Debug.Log("result JsonFromBinaryData: " + res2);

        }
    }
}