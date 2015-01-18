function Waypoint(position, heading) {
    this.position = position;
    this.heading = heading;

    this.setPosition = function (position) { this.position = position; this.notifyPath(); }
    this.setHeading = function (heading) { this.heading = heading; this.notifyPath(); }
    this.getPosition = function () { return this.position; }
    this.getHeading = function () { return this.heading; }

    this.setBelongsToPath = function (path) { this.path = path; }
    this.notifyPath = function () { if (this.path != null) this.path.onWaypointModified(this); }
};

function SingleWaypointSelection() {
    var selected_waypoint = null;
    var listeners = [];

    var notify_listeners = function () {
        for (var i = 0; i < listeners.length; i++) {
            listeners[i].onSelectionChanged(selection);
        }
    }

    this.selectWaypoint = function (waypoint) {
        selected_waypoint = waypoint;
        notify_listeners();
    };

    this.getSelectedWaypoint = function () { return selected_waypoint; }

    this.clear = function () { selected_waypoint = null; notify_listeners(); };

    this.registerListener = function (listener) { listeners.push(listener); }
};

function Path() {
    var waypoints = [];
    var listeners = [];
    var is_collecting_changes = false;

    this.registerListener = function (listener) { listeners.push(listener); }
    this.unregisterListener = function (listener) { listeners.slice(listeners.indexOf(listener), 1); }

    this.beginCollectChanges = function () { is_collecting_changes = true; }
    this.endCollectChanges = function () { is_collecting_changes = false; notifyListeners(); }

    notifyListeners = function () {
        if (is_collecting_changes == false) {
            for (var i = 0; i < listeners.length; i++) {
                var listener = listeners[i];
                listener.onPathModified(this);
            }
        }
    }

    this.appendWaypoint = function (waypoint) {    
        waypoint.setBelongsToPath(this);
        waypoints.push(waypoint);

        notifyListeners();
    }

    this.removeWaypoint = function (waypoint) {
        waypoint.setBelongsToPath(null);
        waypoints.splice(waypoints.indexOf(waypoint), 1);

        notifyListeners();
    }

    this.moveWaypointDown = function (waypoint) {
        index_of_waypoint = waypoints.indexOf(waypoint);
        num_waypoints = waypoints.length;

        if (index_of_waypoint < num_waypoints-1)
        {
            waypoints[index_of_waypoint] = waypoints[index_of_waypoint + 1];
            waypoints[index_of_waypoint + 1] = waypoint;

            notifyListeners();
        }
    }

    this.moveWaypointUp = function (waypoint) {
        index_of_waypoint = waypoints.indexOf(waypoint);
        num_waypoints = waypoints.length;

        if (index_of_waypoint > 0) {
            waypoints[index_of_waypoint] = waypoints[index_of_waypoint - 1];
            waypoints[index_of_waypoint - 1] = waypoint;

            notifyListeners();
        }
    }

    this.getWaypoint = function (index) {
        return waypoints[index];
    }

    this.getNumWaypoints = function () { return waypoints.length; }

    this.onWaypointModified = function (waypoint) {
        notifyListeners();
    }
};
