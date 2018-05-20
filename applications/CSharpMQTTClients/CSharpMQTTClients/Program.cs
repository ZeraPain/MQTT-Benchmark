using System;
using System.Threading;
using uPLibrary.Networking.M2Mqtt.Messages;

namespace uPLibrary.Networking.M2Mqtt
{
    internal class Program
    {
        private static string MQTT_BROKER_ADDRESS = "192.168.178.37";

        private static void Main(string[] args)
        {
            if (args.Length > 0) // publisher
            {
                Publisher();
            }
            else
            {
                Subscriber();
            }
        }

        private static void Publisher()
        {
            Console.WriteLine("Started Publisher");

            // create client instance 
            var client = new MqttClient(MQTT_BROKER_ADDRESS); 
 
            var clientId = Guid.NewGuid().ToString(); 
            client.Connect(clientId);

            byte[] sendBytes =
            {
                0x00, 0x01
            };
 
            // publish a message on "/home/temperature" topic with QoS 2 
            while (true)
            {
                client.Publish("/home/temperature", sendBytes, MqttMsgBase.QOS_LEVEL_EXACTLY_ONCE, false);
                Thread.Sleep(10);
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
            //Console.WriteLine("Received message");
        } 
    }
}
