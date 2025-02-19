#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "CollegeModel.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    CollegeModel model;

    //Sample data
    // College college1;
    // college1.name = "College A";
    // college1.distances = { {"College B", 10.5}, {"College C", 20.0}, {"College A", 0} };
    // college1.souvenirs = { {"Mug", 12.99}, {"T-Shirt", 25.50} };

    // College college2;
    // college2.name = "College B";
    // college2.distances = { {"College A", 10.5}, {"College C", 15.0}, {"College B", 0} };
    // college2.souvenirs = { {"Keychain", 5.99}, {"Notebook", 9.99} };

    // College college3;
    // college3.name = "College C";
    // college3.distances = { {"College A", 20.0}, {"College B", 15.0}, {"College C", 0}};
    // college3.souvenirs = { {"Pen", 2.99}, {"Hat", 18.00} };

    // model.addCollege(college1);
    // model.addCollege(college2);
    // model.addCollege(college3);

    College college1;
    college1.name = "Arizona State University";
    college1.distances = {
        {"MIT", 2636},
        {"Northwestern", 1756},
        {"Ohio State University", 1865},
        {"Saddleback College", 379},
        {"University of Michigan", 1956},
        {"UCI", 375},
        {"UCLA", 397},
        {"University of Oregon", 1343},
        {"University of the Pacific", 720},
        {"University of Wisconsin", 1684}
    };
    college1.souvenirs = {
        {"Mug", 12.99},
        {"T-Shirt", 25.50}
    };
    model.addCollege(college1);

    College college2;
    college2.name = "MIT";
    college2.distances = {
        {"Arizona State University", 2636},
        {"Northwestern", 993},
        {"Ohio State University", 772},
        {"Saddleback College", 2989},
        {"University of Michigan", 803},
        {"UCI", 2986},
        {"UCLA", 2998},
        {"University of Oregon", 3107},
        {"University of the Pacific", 3057},
        {"University of Wisconsin", 1125}
    };
    college2.souvenirs = {
        {"Notebook", 9.99},
        {"Keychain", 5.99}
    };
    model.addCollege(college2);

    College college3;
    college3.name = "Northwestern";
    college3.distances = {
        {"Arizona State University", 1756},
        {"MIT", 993},
        {"Ohio State University", 335},
        {"Saddleback College", 2033},
        {"University of Michigan", 270},
        {"UCI", 2030},
        {"UCLA", 2043},
        {"University of Oregon", 2151},
        {"University of the Pacific", 2102},
        {"University of Wisconsin", 145}
    };
    college3.souvenirs = {
        {"Sweatshirt", 30.00},
        {"Pennant", 15.00}
    };
    model.addCollege(college3);

    College college4;
    college4.name = "Ohio State University";
    college4.distances = {
        {"Arizona State University", 1865},
        {"MIT", 772},
        {"Northwestern", 335},
        {"Saddleback College", 2252},
        {"University of Michigan", 184},
        {"UCI", 2248},
        {"UCLA", 2261},
        {"University of Oregon", 2241},
        {"University of the Pacific", 2412},
        {"University of Wisconsin", 503}
    };
    college4.souvenirs = {
        {"Baseball Cap", 20.00},
        {"Sticker Pack", 7.99}
    };
    model.addCollege(college4);

    College college5;
    college5.name = "Saddleback College";
    college5.distances = {
        {"Arizona State University", 379},
        {"MIT", 2989},
        {"Northwestern", 2033},
        {"Ohio State University", 2252},
        {"University of Michigan", 2249},
        {"UCI", 16},
        {"UCLA", 66},
        {"University of Oregon", 906},
        {"University of the Pacific", 390},
        {"University of Wisconsin", 1981}
    };
    college5.souvenirs = {
        {"Tote Bag", 18.50},
        {"Lanyard", 6.99},
        {"Hoodie", 39.99},
        {"Sunglasses", 14.99},
        {"Baseball Cap", 20.00},
        {"Sticker Pack", 7.99},
        {"Scarf", 22.00},
        {"Keychain", 5.99}
    };
    model.addCollege(college5);

    College college6;
    college6.name = "University of Michigan";
    college6.distances = {
        {"Arizona State University", 1956},
        {"MIT", 803},
        {"Northwestern", 270},
        {"Ohio State University", 184},
        {"Saddleback College", 2249},
        {"UCI", 2245},
        {"UCLA", 2252},
        {"University of Oregon", 2347},
        {"University of the Pacific", 2318},
        {"University of Wisconsin", 391}
    };
    college6.souvenirs = {
        {"Scarf", 22.00},
        {"Backpack", 45.99}
    };
    model.addCollege(college6);

    College college7;
    college7.name = "UCI";
    college7.distances = {
        {"Arizona State University", 375},
        {"MIT", 2986},
        {"Northwestern", 2030},
        {"Ohio State University", 2248},
        {"Saddleback College", 16},
        {"University of Michigan", 2245},
        {"UCLA", 51},
        {"University of Oregon", 899},
        {"University of the Pacific", 383},
        {"University of Wisconsin", 1978}
    };
    college7.souvenirs = {
        {"Notebook", 10.99},
        {"Water Bottle", 19.99}
    };
    model.addCollege(college7);

    College college8;
    college8.name = "UCLA";
    college8.distances = {
        {"Arizona State University", 397},
        {"MIT", 2998},
        {"Northwestern", 2043},
        {"Ohio State University", 2261},
        {"Saddleback College", 66.3},
        {"University of Michigan", 2252},
        {"UCI", 51.5},
        {"University of Oregon", 849},
        {"University of the Pacific", 333},
        {"University of Wisconsin", 1991}
    };
    college8.souvenirs = {
        {"Hoodie", 39.99},
        {"Sunglasses", 14.99}
    };
    model.addCollege(college8);

    College college9;
    college9.name = "University of Oregon";
    college9.distances = {
        {"Arizona State University", 1343},
        {"MIT", 3107},
        {"Northwestern", 2151},
        {"Ohio State University", 2241},
        {"Saddleback College", 906},
        {"University of Michigan", 2347},
        {"UCI", 899},
        {"UCLA", 849},
        {"University of the Pacific", 517},
        {"University of Wisconsin", 2111}
    };
    college9.souvenirs = {
        {"Sweatpants", 35.99},
        {"Water Bottle", 17.99}
    };
    model.addCollege(college9);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("collegeModel", &model);  // Expose collegeModel
    engine.loadFromModule("College", "Main");

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
