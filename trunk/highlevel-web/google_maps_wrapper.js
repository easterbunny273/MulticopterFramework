function MapWrapper(container_element, _path, _selection)
{
    var map = null; 
    var polyline = null;
    var path = _path;
    var selection = _selection;
    var markers = [];
    var is_updating = false;

        //var markers_for_waypoints = new Map();
        var waypoints_for_markers = new Map();

        this.onPathModified = function (path) { update(); }
        this.onSelectionChanged = function (selection) { update(); }

        var update = function()
        {
            if (is_updating == false)
            {
                is_updating = true;

                clearPolyline();
                clearMarkers();

                createMarkers();
                createPolyline();

                is_updating = false;
            }
        }

    
        var createMarkers = function()
        {
            for (var i = 0; i < path.getNumWaypoints(); i++) {
                var waypoint = path.getWaypoint(i);
                var new_marker = createMarkerForWaypoint(waypoint);

                markers.push(new_marker);
                waypoints_for_markers.set(new_marker, waypoint);
            }
        }

        var createPolyline = function()
        {
            var vertices = [];
            var latLngBounds = new google.maps.LatLngBounds();

            for (var i = 0; i < path.getNumWaypoints() ; i++) {
                var waypoint = path.getWaypoint(i);
                var position = waypoint.getPosition();
                vertices.push(position);
                latLngBounds.extend(position);
            }

            // Creates the polyline object
            polyline = new google.maps.Polyline({
                map: map,
                path: vertices,
                strokeColor: '#FF0000',
                strokeOpacity: 0.7,
                strokeWeight: 2
            });
        }

        var clearPolyline = function()
        {
            if (polyline != null) {
                polyline.setMap(null);
                delete polyline;
            }
        }

        var clearMarkers = function()
        {
            for (var i = 0; i < markers.length; i++) {
                markers[i].setMap(null);
            }
            waypoints_for_markers.clear();
        }

        var createMarkerForWaypoint = function(waypoint)
        {
            var location = waypoint.getPosition();
            var marker = new google.maps.Marker({
                map: map,
                position: location,
                draggable: true
            });

            pinColor = (waypoint == selection.getSelectedWaypoint()) ? "FF0000" : "00FF00";

            var pinImage = new google.maps.MarkerImage("http://chart.apis.google.com/chart?chst=d_map_pin_letter&chld=%E2%80%A2|" + pinColor,
                new google.maps.Size(21, 34),
                new google.maps.Point(0, 0),
                new google.maps.Point(10, 34));
            marker.setIcon(pinImage);

            // connect event handlers
            google.maps.event.addListener(marker, 'click', function () { selection.selectWaypoint(waypoints_for_markers.get(marker)); });
            google.maps.event.addListener(marker, 'rightclick', function () { path.removeWaypoint(waypoints_for_markers.get(marker)); });
            google.maps.event.addListener(marker, 'dragend', function (event) {
                is_updating = true;
                selection.selectWaypoint(waypoints_for_markers.get(marker));
                waypoints_for_markers.get(marker).setPosition(marker.getPosition());
                is_updating = false;

                update();
                
            });

            return marker;
        }


    // execute on construction
        map = new google.maps.Map(container_element, {
            zoom: 16,
            center: new google.maps.LatLng(48.223265, 16.422191),
            mapTypeId: google.maps.MapTypeId.SATELLITE,
            streetViewControl: false,
            tilt: 0,				// we do not allow 45° tilting, because there is a minor mismatch between seen and real gps coordinates
            draggableCursor: 'crosshair'
        });

        google.maps.event.addListener(map, 'click', function (event) {
            var new_waypoint = new Waypoint(event.latLng, -1);
            selection.selectWaypoint(new_waypoint);
            path.appendWaypoint(new_waypoint);
        });

        //$(container_element).append()
        path.registerListener(this);
        selection.registerListener(this);

        // call update once for initialization
        this.onPathModified(path);
}