using System;
using System.Threading;
using uPLibrary.Networking.M2Mqtt.Messages;

namespace uPLibrary.Networking.M2Mqtt
{
    internal class Program
    {
        private static string MQTT_BROKER_ADDRESS = "localhost";

        private static void Main(string[] args)
        {
            if (args.Length == 2) // publisher
            {
                Publisher(args);
            }
            else if(args.Length == 0)
            {
                Subscriber();
            } else
            {
                Console.WriteLine("Parameters: size, interval");
            }
        }

        private static void Publisher(string[] args)
        {
            Console.WriteLine("Started Publisher");

            int size = Int32.Parse(args[0]);
            int interval = Int32.Parse(args[1]);
            // create client instance 
            var client = new MqttClient(MQTT_BROKER_ADDRESS); 
 
            var clientId = Guid.NewGuid().ToString(); 
            client.Connect(clientId);
            var sendBytes = new byte[size];
            for(int i = 0; i < size; ++i)
            {
                sendBytes[i] = 1;
            }
            // publish a message on "/home/temperature" topic with QoS 2 
            while (true)
            {
                client.Publish("/home/temperature", sendBytes, MqttMsgBase.QOS_LEVEL_EXACTLY_ONCE, false);
                Thread.Sleep(interval);
            }
        }

        private static void Subscriber()
        {
            Console.WriteLine("Started Subscriber");

            // create client instance 
            var client = new MqttClient(MQTT_BROKER_ADDRESS); 
 
            // register to message received 
            client.MqttMsgPublishReceived += client_MqttMsgPublishReceived; 
 
            var clientId = Guid.NewGuid().ToString(); 
            client.Connect(clientId); 
 
            // subscribe to the topic "/home/temperature" with QoS 2 
            client.Subscribe(new[] { "/home/temperature" }, new[] { MqttMsgBase.QOS_LEVEL_EXACTLY_ONCE });
        }

        private static void client_MqttMsgPublishReceived(object sender, MqttMsgPublishEventArgs e) 
        { 
            // handle message received 
            // Console.WriteLine("Received message");
        } 
    }
}
