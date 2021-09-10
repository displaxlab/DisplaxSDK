using System;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using UnityEngine;
using DPXNetwork;
using DPXUtils;

namespace DPXNetwork
{
    [Serializable]
    public class DPXServiceBaseConnInfo
    {
        public string frame_type;
        public int server_port;
        public string type;
        public string id;
        public int communication_port;
    }

    public class DPXNetConnection
    {

        public delegate void OnConnected();
        public static event OnConnected onConnected;

        public delegate void OnDisconnected();
        public static event OnDisconnected onDisconnected;

        public delegate void OnNewData(byte[] data, int length);
        public static event OnNewData onNewData;

        private enum connectionState { undef, connected, disconnected }
        private connectionState connState = connectionState.undef;
        private connectionState lastConnState = connectionState.undef;

        private float waitTime = 2.0f;
        private float timer = 0.0f;

        private static DPXNetUDPClient sendUdpClient = new DPXNetUDPClient();
        private static DPXNetTCPClient sendTcpClient = new DPXNetTCPClient();

        private DPXServiceBaseConnInfo baseConnInfo = new DPXServiceBaseConnInfo();

        private DPXServiceBaseConnInfo baseConnTCPInfo = new DPXServiceBaseConnInfo();

        private QJsonDocTranslator jsonTranslator = new QJsonDocTranslator();

        private string remoteHost = "127.0.0.1";
        private int remotePort = 45001;

        public string GetRemoteHost()
        {
            return remoteHost.ToString();
        }
        public void SetRemoteHost(string host)
        {
            remoteHost = host;
        }

        public int GetRemotePort()
        {
            return remotePort;
        }

        public void SetRemotePort(int port)
        {
            remotePort = port;
        }

        public void mustStop()
        {
            sendTcpClient.mustStop = true;
        }

        public void Init()
        {
            ConnInit();
            ConnSendDiscovery();
        }

        private void ConnInit()
        {
            String ipaddrs = string.Empty;
            String hostName = string.Empty;
            hostName = Dns.GetHostName();
            IPHostEntry myIP = Dns.GetHostEntry(hostName);
            IPAddress[] address = myIP.AddressList;
            for (int i = 0; i < address.Length; i++)
            {
                if (address[i].AddressFamily == AddressFamily.InterNetwork)
                    ipaddrs += address[i].ToString();
            }
            //if there's no IPv4 addresses, try IPv6 addresses
            if (ipaddrs == "")
            {
                for (int i = 0; i < address.Length; i++)
                {
                    if (address[i].AddressFamily == AddressFamily.InterNetworkV6)
                        ipaddrs += address[i].ToString();
                }
            }

            lastConnState = connectionState.disconnected;
            connState = connectionState.disconnected;

            baseConnInfo.server_port = 0;
            baseConnInfo.type = "DesktopConnect";
            baseConnInfo.id = "DisplaxDesktopConnect" + DateTimeOffset.Now.ToUnixTimeMilliseconds() + "0000" + ipaddrs + this.GetHashCode().ToString();

            DPXNetTCPClient.onReadyRead -= TCPNewData;
            DPXNetTCPClient.onConnected -= TCPConnected;
            DPXNetTCPClient.onDisconnected -= TCPDisconnected;
            DPXNetTCPClient.onConnected += TCPConnected;
            DPXNetTCPClient.onDisconnected += TCPDisconnected;

            sendUdpClient.SetRemoteHost(remoteHost);
            sendUdpClient.SetRemotePort(remotePort);
            sendUdpClient.InitUDPConnection();

            if (DPXConstants.DPX_DEBUG_INFO)
                Debug.Log("Network started.");
        }

        private void ConnSendDiscovery()
        {
            baseConnInfo.frame_type = "discovery";
            baseConnInfo.communication_port = 0;

            string json = JsonUtility.ToJson(baseConnInfo);

            byte[] ux = jsonTranslator.JsonToBinaryData(json);

            DPXNetUDPClient.onReadyRead -= ProcessDiscoveryData;
            DPXNetUDPClient.onReadyRead += ProcessDiscoveryData;
            sendUdpClient.SendUDPMessage(ux);

            if (DPXConstants.DPX_DEBUG_INFO)
                Debug.Log("Network discovery...");
        }

        private void ProcessDiscoveryData()
        {
            baseConnTCPInfo = JsonUtility.FromJson<DPXServiceBaseConnInfo>(jsonTranslator.JsonFromBinaryData(sendUdpClient.DataBuffer(), sendUdpClient.DataLength()));

            DPXNetUDPClient.onReadyRead -= ProcessDiscoveryData;


            if (!sendTcpClient.IsConnected())
            {
                sendTcpClient.SetRemoteHost(remoteHost);
                sendTcpClient.SetRemotePort(baseConnTCPInfo.server_port);
                sendTcpClient.ConnectToTcpServer();
            }
        }

        private void SendConnInfo()
        {
            baseConnInfo.frame_type = "connection_info";
            baseConnInfo.communication_port = sendTcpClient.GetCurrentLocalPort();

            string json = JsonUtility.ToJson(baseConnInfo);

            byte[] ux = jsonTranslator.JsonToBinaryData(json);

            sendUdpClient.SendUDPMessage(ux, false);
        }

        private void TCPConnected()
        {
            SendConnInfo();

            DPXNetTCPClient.onReadyRead -= TCPNewData;
            DPXNetTCPClient.onReadyRead += TCPNewData;

            connState = connectionState.connected;
        }

        private void TCPDisconnected()
        {
            connState = connectionState.disconnected;
            DPXNetTCPClient.onReadyRead -= TCPNewData;
            if (sendTcpClient.IsConnected())
                sendTcpClient.Disconnect();
        }

        
        private void TCPNewData()
        {
            if(onNewData != null)
                onNewData(sendTcpClient.DataBuffer(), sendTcpClient.DataLength());
        }

        public bool SendData(byte[] data)
        {
            if(connState == connectionState.connected)
            {
                sendTcpClient.SendImmediate(data);
                return true;
            }
            else
                return false;
        }

        // Update is called once per frame
        public void UpdateConnection()
        {
            timer += Time.deltaTime;

            // Check if we have reached beyond 2 seconds.
            // Subtracting two is more accurate over time than resetting to zero.
            if (timer > waitTime)
            {
                // Remove the recorded 2 seconds.
                timer = timer - waitTime;

                if (connState != lastConnState && connState == connectionState.disconnected)
                {
                    if (DPXConstants.DPX_DEBUG_INFO)
                        Debug.Log("Disconnected");

                    if (onDisconnected != null)
                        onDisconnected();
                }
                lastConnState = connState;

                // check current UDP connection, if disconnected
                if (connState == connectionState.disconnected)
                    ConnSendDiscovery();
            }

            if (connState != lastConnState && connState == connectionState.connected)
            {
                lastConnState = connState;

                if (DPXConstants.DPX_DEBUG_INFO)
                    Debug.Log("Connected");

                if (onConnected != null)
                    onConnected();
            }

        }

    }
}