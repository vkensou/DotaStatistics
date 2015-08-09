#ifndef HEROITEMS_H
#define HEROITEMS_H

#include <QString>
#include <QHash>
#include <functional>

class DataBaseManager;
class WebDataDownloader;

//hero winning rate with item
class HeroItems
{
public:
    const QString & getName() const{return m_name;}
    HeroItems(const QString &name);

    void download();
    void load(bool force_download = false);
    void save();
    void saveasxml();

    struct ItemRateAndUsed
    {
        ItemRateAndUsed(const QString &name, int used, double rate)
            :name(name), used(used), rate(rate)
        {
        }

        ItemRateAndUsed(const QString &name, int used, double rate, double x2)
            :name(name), used(used), rate(rate), x2(x2)
        {
        }

        QString name;
        int used;
        double rate;
        double x2{0};

        bool operator < (const ItemRateAndUsed &a) const
        {
            return a.name < name;
        }
    };

    int getItemsCount() const;
    void for_each_items(std::function<void(ItemRateAndUsed &)> func);
    void for_each_items(std::function<void(const ItemRateAndUsed &)> func) const;

private:
    void calcX2(int heroused, float herorate);
    double getX2(int heroused, double herorate, int itemused, double itemrate);
    void clear();
    void addItem(const QString &name, int used, double rate, double x2);
    QString getHeroItemsFilename();
    QString getHeroItemsX2Filename();

    QString m_name;
    QString m_chinese_name;

    QHash<QString, ItemRateAndUsed> m_list;


    friend DataBaseManager;
	friend WebDataDownloader;
};

#endif // HEROITEMS_H
