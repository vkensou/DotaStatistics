#include "datamanager.h"
#include "dataconfig.h"
#include "heroesusedandrate.h"
#include "heroitems.h"
#include <functional>
#include <algorithm>

DataManager::DataManager()
    :query(db)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("dota2satistics.db");

}

bool DataManager::opendb()
{
    return db.open();
}

void DataManager::closedb()
{
    db.close();
}

bool DataManager::loadHeroesUsedAndRate(HeroesUsedAndRate &hur, const DataConfig &config)
{
    static QString tablenamefmt = "herousedandrate%1";
    QString tablename = tablenamefmt.arg(config.getFileParams());

    QSqlTableModel model(0, db);
    model.setTable(tablename);
    model.select();

    if(model.rowCount() == 0)
        return false;
    else
    {
        hur.list.clear();
        for(int i = 0; i < model.rowCount(); ++i)
        {
            auto record = model.record(i);
            auto name = record.value("name").toString();
            auto used = record.value("used").toInt();
            auto rate = record.value("rate").toDouble();
            hur.addHero(name, used, rate);
        }
        return true;
    }
}

void DataManager::saveHeroesUsedAndRate(const HeroesUsedAndRate &hur, const DataConfig &config)
{
    static QString tablenamefmt = "herousedandrate%1";
    static QString sqlcreate = "CREATE TABLE IF NOT EXISTS %1 (name TEXT NOT NULL, used INTEGER NOT NULL, rate DOUBLE NOT NULL);";
    static QString sqldelete = "DELETE from %1;";
    static QString sqlinsert = "INSERT INTO %1(name, used, rate) VALUES('%2', %3, %4);";
    QString tablename = tablenamefmt.arg(config.getFileParams());

    QSqlError error;
    QString errortext;

    db.exec(sqlcreate.arg(tablename));
    db.exec(sqldelete.arg(tablename));

    db.transaction();
    auto func = [this, &tablename](const HeroesUsedAndRate::HeroRateAndUsed &hero)
    {
        QString sql = sqlinsert.arg(tablename).arg(hero.name).arg(hero.used).arg(hero.rate);
        db.exec(sql);
    };
    std::for_each(hur.list.begin(), hur.list.end(), func);
    db.commit();
}

bool DataManager::loadHeroItems(HeroItems &hero, const DataConfig &config)
{
    static QString tablenamefmt = "%1%2";
    QString tablename = tablenamefmt.arg(hero.getName()).arg(config.getFileParams());

    QSqlTableModel model(0, db);
    model.setTable(tablename);
    model.select();

    if(model.rowCount() == 0)
        return false;
    else
    {
        hero.clear();
        for(int i = 0; i < model.rowCount(); ++i)
        {
            auto record = model.record(i);
            auto name = record.value("name").toString();
            auto used = record.value("used").toInt();
            auto rate = record.value("rate").toDouble();
            auto x2 = record.value("x2").toDouble();
            hero.addItem(name, used, rate, x2);
        }
        return true;
    }
}

void DataManager::saveHeroItems(const HeroItems &hero, const DataConfig &config)
{
    static QString tablenamefmt = "%1%2";
    static QString sqlcreate = "CREATE TABLE IF NOT EXISTS %1 (name TEXT NOT NULL, used INTEGER NOT NULL, rate DOUBLE NOT NULL, x2 DOUBLE NOT NULL);";
    static QString sqldelete = "DELETE from %1;";
    static QString sqlinsert = "INSERT INTO %1(name, used, rate, x2) VALUES('%2', %3, %4, %5);";

    QString tablename = tablenamefmt.arg(hero.getName()).arg(config.getFileParams());

    db.exec(sqlcreate.arg(tablename));
    db.exec(sqldelete.arg(tablename));

    db.transaction();
    auto func = [this, &tablename](const HeroItems::ItemRateAndUsed &item)
    {
        QString sql = sqlinsert.arg(tablename).arg(item.name).arg(item.used).arg(item.rate).arg(item.x2);
        db.exec(sql);
    };
    hero.for_each_items(func);
    db.commit();
}
