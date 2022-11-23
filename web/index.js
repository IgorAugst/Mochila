function loadMap(){
    const mapOptions = {
        center: {
          lat: 0,
          lng: 0,
        },
        zoom: 4,
    };
    console.log("Loading map");
    const loader = new google.maps.plugins.loader.Loader({
        apiKey: "AIzaSyCdLrku76jCwReDCe26PjPvDTP3WFO7hIg",
        version: "weekly"
    });
    
    loader.loadCallback(e => {
        if (e) {
          console.log(e);
        } else {
          new google.maps.Map(document.getElementById("map"), mapOptions);
        }
      });
}