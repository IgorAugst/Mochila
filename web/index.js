const iotsdk = require('aws-iot-device-sdk-v2');
const iot = iotsdk.iot;
const mqtt = iotsdk.mqtt;
const AWS = require('aws-sdk');
const $ = require('jquery');

//setTimeout(() => {
//	document.getElementById('iframe').src = "https://www.google.com/maps/embed/v1/place?q=ibirapuera2&key=AIzaSyCdLrku76jCwReDCe26PjPvDTP3WFO7hIg";
//}, 10000);

async function connect_websocket(){
	return new Promise((resolve, reject) => {
		let config =
		iot.AwsIotMqttConnectionConfigBuilder.new_default_builder()
			.with_clean_session(true)
			.with_client_id(`web(${new Date()})`)
			.with_endpoint("a12s85dpg0xou0-ats.iot.sa-east-1.amazonaws.com")
			.with_custom_authorizer("", "autorizador")
			.with_keep_alive_seconds(60)
			.build();

		const client = new mqtt.MqttClient();

		const connection = client.new_connection(config);

		connection.on('connect', () => {
			console.log('connected');
			resolve(connection);
		});

        connection.on("interrupt", (error) => {
            log(`Connection interrupted: error=${error}`);
        });

        connection.on("resume", (return_code, session_present) => {
            log(`Resumed: rc: ${return_code} existing session: ${session_present}`);
        });

        connection.on("disconnect", () => {
            log("Disconnected");
        });

        connection.on("error", (error) => {
            reject(error);
        });

		connection.connect();
	});

}

var connection;
var updateAcceptedTopic = "$aws/things/mochila/shadow/name/location/update/accepted";
var getAcceptedTopic = "$aws/things/mochila/shadow/name/location/get/accepted";
var getTopic = "$aws/things/mochila/shadow/name/location/get";

async function main(){
	console.log("Connecting to websocket");
	connection = await connect_websocket();
	connection.subscribe(updateAcceptedTopic, 0, (topic, payload) => {messageCallback(topic, payload)});
	connection.subscribe(getAcceptedTopic, 0, (topic, payload) => {messageCallback(topic, payload)});
	setTimeout(() => {connection.publish(getTopic, "", 0)}, 1000);
	
}

function messageCallback(topic, payload){
	console.log("Message received on topic: " + topic);
	if (topic == updateAcceptedTopic || topic == getAcceptedTopic) {
		const decoder = new TextDecoder('utf8');
		let message = decoder.decode(new Uint8Array(payload));
		let json = JSON.parse(message);
		var lat = json["state"]["reported"]["lat"];
		var lon = json["state"]["reported"]["lon"];
		updateMap(lat, lon);
	}

	if(topic == getAcceptedTopic){
		connection.unsubscribe(getAcceptedTopic);
	}
}

function updateMap(lat, lon){;
	document.getElementById('iframe').src = "https://www.google.com/maps/embed/v1/place?q=" + lat + "," + lon + "&key=AIzaSyCdLrku76jCwReDCe26PjPvDTP3WFO7hIg";
}

main();