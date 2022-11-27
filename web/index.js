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
		connection.connect();
	});

}

var connection;

var updateMapAcceptedTopic = "$aws/things/mochila/shadow/name/location/update/accepted";
var getMapAcceptedTopic = "$aws/things/mochila/shadow/name/location/get/accepted";
var getMapTopic = "$aws/things/mochila/shadow/name/location/get";

var updateTempAcceptedTopic = "$aws/things/mochila/shadow/name/weather/update/accepted";
var getTempAcceptedTopic = "$aws/things/mochila/shadow/name/weather/get/accepted";
var getTempTopic = "$aws/things/mochila/shadow/name/weather/get";

async function main(){
	console.log("Connecting to websocket");
	connection = await connect_websocket();
	
	connection.subscribe(updateMapAcceptedTopic, 0, (topic, payload) => {messageCallback(topic, payload)});
	connection.subscribe(getMapAcceptedTopic, 0, (topic, payload) => {messageCallback(topic, payload)});
	
	connection.subscribe(updateTempAcceptedTopic, 0, (topic, payload) => {messageCallback(topic, payload)});
	connection.subscribe(getTempAcceptedTopic, 0, (topic, payload) => {messageCallback(topic, payload)});
	
	setTimeout(() => {
		connection.publish(getMapTopic, "", 0);
		connection.publish(getTempTopic, "", 0);
	}, 1000);
	
}

function messageCallback(topic, payload){
	console.log("Message received on topic: " + topic);
	if (topic == updateMapAcceptedTopic || topic == getMapAcceptedTopic) {
		const decoder = new TextDecoder('utf8');
		let message = decoder.decode(new Uint8Array(payload));
		let json = JSON.parse(message);
		var lat = json["state"]["reported"]["lat"];
		var lon = json["state"]["reported"]["lon"];
		updateMap(lat, lon);
	}

	if(topic == getMapAcceptedTopic){
		connection.unsubscribe(getMapAcceptedTopic);
	}

	if (topic == updateTempAcceptedTopic || topic == getTempAcceptedTopic) {
		const decoder = new TextDecoder('utf8');
		let message = decoder.decode(new Uint8Array(payload));
		let json = JSON.parse(message);
		var temp = json["state"]["reported"]["temp"];
		var hum = json["state"]["reported"]["hum"];
		updateTemp(temp, hum);
	}

	if(topic == getTempAcceptedTopic){
		connection.unsubscribe(getTempAcceptedTopic);
	}
}

function updateMap(lat, lon){;
	document.getElementById('iframe').src = "https://www.google.com/maps/embed/v1/place?q=" + lat + "," + lon + "&key=AIzaSyCdLrku76jCwReDCe26PjPvDTP3WFO7hIg";
}

function updateTemp(temp, hum){
	$("#temp").text(temp + "°C");
	$("#hum").text(hum + "%");
}

main();