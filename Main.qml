import QtQuick 6.6
import QtQuick.Controls 6.6

ApplicationWindow
{
    visible: true
    width: 1400
    height: 800
    title: "College Touring"
    color: "#212121"

    property real spacing: width / 20
    property real fontScale: Math.min(width, height) * 0.025

    ListModel { id: collegeModel }
    ListModel { id: distanceModel }
    ListModel { id: souvenirModel }

    function loadColleges()
    {
        console.log("Fetching colleges from database...");
        var data = dbManager.fetchColleges();

        if (!data || data.length === 0)
        {
            console.log("No colleges found in database.");
            return;
        }

        console.log("Fetched colleges count:", data.length);
        collegeModel.clear();

        let uniqueColleges = new Set();

        for (var i = 0; i < data.length; i++)
        {
            if (!uniqueColleges.has(data[i].name))
            {
                uniqueColleges.add(data[i].name);
                collegeModel.append({ "name": data[i].name });
                console.log("Added college:", data[i].name);
            }
        }

        if (collegeModel.count > 0)
        {
            referenceCollegeDropdown.currentIndex = 0;
            loadCollegeData(collegeModel.get(0).name);
        }
    }

    function loadCollegeData(collegeName)
    {
        console.log("Fetching data for:", collegeName);
        distanceModel.clear();
        souvenirModel.clear();

        // **Fetch distances**
        var distances = dbManager.fetchDistances(collegeName);
        if (distances && distances.length > 0)
        {
            for (var i = 0; i < distances.length; i++)
            {
                distanceModel.append
                ({
                    "ending_college": distances[i].ending_college,
                    "distance": distances[i].distance
                });
            }
        }

        // **Fetch souvenirs**
        var souvenirs = souvenirDB.fetchSouvenirs(collegeName);
        if (souvenirs && souvenirs.length > 0)
        {
            for (var j = 0; j < souvenirs.length; j++)
            {
                souvenirModel.append
                ({
                    "souvenir": souvenirs[j].souvenir,
                    "price": souvenirs[j].price.replace("$", "") // Ensure prices are formatted properly
                });
            }
        }

        console.log("Data loaded for:", collegeName, "- Distances:", distanceModel.count, "- Souvenirs:", souvenirModel.count);
    }

    Component.onCompleted:
    {
        console.log("Initializing college list...");
        loadColleges();
    }

    Rectangle
    {
        id: contentArea
        anchors.fill: parent
        anchors.margins: spacing
        color: "#212121"

        ComboBox
        {
            id: referenceCollegeDropdown
            anchors.top: parent.top
            anchors.left: parent.left
            width: parent.width / 6
            model: collegeModel
            textRole: "name"

            onCurrentTextChanged:
            {
                if (currentIndex >= 0)
                {
                    var selectedCollege = referenceCollegeDropdown.currentText;
                    console.log("Selected College: ", selectedCollege);
                    loadCollegeData(selectedCollege);
                }
            }
        }

        // **Distance List**
        Flickable
        {
            id: flickable1
            anchors.top: referenceCollegeDropdown.bottom
            anchors.topMargin: spacing
            anchors.left: parent.left
            width: (parent.width - 2 * spacing) / 3
            height: parent.height - referenceCollegeDropdown.height - 2 * spacing
            contentWidth: width
            contentHeight: collegeList.height

            ListView
            {
                id: collegeList
                width: parent.width
                height: contentHeight
                model: distanceModel

                delegate: Rectangle
                {
                    width: parent.width
                    height: 60
                    color: "#333333"
                    border.color: "#555555"
                    radius: 5

                    Text
                    {
                        text: model.ending_college + " - Distance: " + model.distance + " mi"
                        color: "#E0E0E0"
                        font.pixelSize: fontScale
                        anchors.centerIn: parent
                    }
                }
            }
        }

        // **Souvenirs List**
        Flickable
        {
            id: flickable2
            anchors.top: referenceCollegeDropdown.bottom
            anchors.topMargin: spacing
            anchors.left: flickable1.right
            anchors.leftMargin: spacing
            width: (parent.width - 2 * spacing) / 3
            height: parent.height - referenceCollegeDropdown.height - 2 * spacing
            contentWidth: width
            contentHeight: souvenirList.height

            ListView
            {
                id: souvenirList
                width: parent.width
                height: contentHeight
                model: souvenirModel

                delegate: Rectangle
                {
                    width: parent.width
                    height: 50
                    color: "#444444"
                    border.color: "#666666"
                    radius: 5

                    Text
                    {
                        text: model.souvenir + " - $" + model.price
                        color: "#E0E0E0"
                        font.pixelSize: fontScale
                        anchors.centerIn: parent
                    }
                }
            }
        }
    }
}
