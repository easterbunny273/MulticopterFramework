function Html5ListWrapper(container_element, _path, _selection) {
    var path = _path;
    var selection = _selection;
    var is_updating = false;

    var handled_waypoints = [];
    var next_free_id = 0;

    this.onPathModified = function (path) { update(); }
    this.onSelectionChanged = function (selection) { update(); }

    var test_path_has_waypoint = function (searched_waypoint) {
        var has_waypoint = false;

        for (var i = 0; i < path.getNumWaypoints() ; i++) {
            var waypoint = path.getWaypoint(i);
            if (waypoint == searched_waypoint)
                has_waypoint = true;
        }

        return has_waypoint;
    }

    var test_is_waypoint_handled = function (searched_waypoint) {
        for (var i = 0; i < handled_waypoints.length; i++) {
            var waypoint = handled_waypoints[i]["waypoint"];
            if (waypoint == searched_waypoint)
                return true;
        }
        return false;
    }

    var removeRemovedWaypoints = function () {
        var data_to_remove = [];

        for (var i = 0; i < handled_waypoints.length; i++)
        {
            /*var div_id = handled_waypoints[i]["div_id"];
            var waypoint = handled_waypoints[i]["waypoint"];

            var waypoint_still_exists = test_path_has_waypoint(waypoint);

            if (waypoint_still_exists == false) {
                data_to_remove.push(handled_waypoints[i]);
            }*/

            // We cannot handle reordered waypoints with the upper snippet,
            // so we remove *all* waypoints and force re-adding them.
            data_to_remove.push(handled_waypoints[i]);
        }

       
        for (var i = 0; i < data_to_remove.length; i++)
        {
            var div_id = data_to_remove[i]["div_id"];

            $("#" + div_id).remove();

            handled_waypoints.splice(handled_waypoints.indexOf(data_to_remove[i]), 1);
        }
    }

    var addNewWaypoints = function () {
        for (var i = 0; i < path.getNumWaypoints() ; i++) {
            var waypoint = path.getWaypoint(i);
            var waypoint_handled = test_is_waypoint_handled(waypoint);

            if (waypoint_handled == false) {
                var div_id = "_" + (next_free_id++);

                // add new element
                $("#path_table").append("<tr id=\"" + div_id + "\"><td id=\"position\"></td><td id=\"heading\"></td><td id=\"commands\"></td></tr>");

                // add command buttons
                    // UP
                    $("#" + div_id + " #commands").append("<input type=\"button\" value=\"Up\" id=\"up\"></input>");
                    $("#" + div_id + " #commands #up").click(waypoint, function (eventdata) { var waypoint_to_move = eventdata.data; path.moveWaypointUp(waypoint_to_move); });

                    // DOWN
                    $("#" + div_id + " #commands").append("<input type=\"button\" value=\"Down\" id=\"down\"></input>");
                    $("#" + div_id + " #commands #down").click(waypoint, function (eventdata) { var waypoint_to_move = eventdata.data; path.moveWaypointDown(waypoint_to_move); });

                    // remove
                    $("#" + div_id + " #commands").append("<input type=\"button\" value=\"X\" id=\"delete\"></input>");
                    $("#" + div_id + " #commands #delete").click(waypoint, function (eventdata) { var waypoint_to_remove = eventdata.data; path.removeWaypoint(waypoint_to_remove); });

                    $("#" + div_id).click(waypoint, function (eventdata) { var waypoint_to_select = eventdata.data; selection.selectWaypoint(waypoint_to_select); });


                handled_waypoints.push({ "waypoint": waypoint, "div_id": div_id });
            }
        }
    }

    var updateWaypoints = function () {
        for (var i = 0; i < handled_waypoints.length; i++) {
            var div_id = handled_waypoints[i]["div_id"];
            var waypoint = handled_waypoints[i]["waypoint"];
            var is_selected = (selection.getSelectedWaypoint() == waypoint);

            // set position
            $("#" + div_id + " #position").empty();
            $("#" + div_id + " #position").append(String(waypoint.getPosition()));

            // set heading
            $("#" + div_id + " #heading").empty();
            $("#" + div_id + " #heading").append(waypoint.getHeading());

            if (is_selected)
            {
                $("#" + div_id).css('background-color', 'red');
            }
        }
    }

    var update = function () {
        if (is_updating == false) {
            is_updating = true;

            removeRemovedWaypoints();
            addNewWaypoints();
            updateWaypoints();

            is_updating = false;
        }
    }

    /// CONSTRUCTION CODE
    $(container_element).append("<table id=\"path_table\"><thead><td id=\"position\">Position</td><td id=\"heading\">Heading</td><td id=\"commands\">Commands</td></thead></table>");

    // register as listener
    path.registerListener(this);
    selection.registerListener(this);

    // update on initialization
    update();
};