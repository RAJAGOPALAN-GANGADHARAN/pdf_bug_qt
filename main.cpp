#include <qwebpage.h>
#include <qwebframe.h>
#include <QPrinter>
#include <unistd.h>
#include <QApplication>
#include <QObject>
#include <QFile>

QString takeOptionValue(QStringList* arguments, int index)
{
    QString result;

    if (++index < arguments->count() && !arguments->at(index).startsWith("-"))
        result = arguments->takeAt(index);

    return result;
}

class TestApp: public QObject
{
    Q_OBJECT
    public:
    TestApp(QWebFrame* frame, QString output)
    :m_frame(frame),
    m_output(output)
    {}
    public Q_SLOTS:
    void print(bool ok)
    {
        if(!ok)
        {
            qDebug() << "Couldn't load the given url, please make sure this is a correct url with protocol";
            QApplication::exit(1);
        }
        QFile file(m_output);
        if(!file.open(QIODevice::ReadWrite))
        {
            qDebug() << "Couldn't create the file try creating it manually and re run the program";
            QApplication::exit(1);
        }

        QRect rect = m_frame->geometry();
        QPrinter painter;
        painter.setOrientation(QPrinter::Landscape);
        painter.setPaperSize(QSize(rect.width(),rect.height()),QPrinter::Point);
        painter.setFullPage(true);
        painter.setOutputFormat(QPrinter::PdfFormat);
        painter.setOutputFileName(m_output);
        m_frame->print(&painter);
        
        QApplication::quit();
    }
    private:
        QWebFrame *m_frame;
        QString m_output;
};
int main(int argc,char** argv)
{
    QApplication qa(argc,argv);
    
    QStringList args = QApplication::arguments();
    QString urlOption(takeOptionValue(&args, args.indexOf("--url")));
    QString outputOption(takeOptionValue(&args, args.indexOf("--output")));

    QWebPage page;
    page.setViewportSize(QSize(1920, 1080));
    QUrl url = QUrl(urlOption);

    TestApp app(page.mainFrame(), outputOption);
    QObject::connect(page.mainFrame(), &QWebFrame::loadFinished,
            &app, &TestApp::print);
    page.mainFrame()->load(url);
    
    return qa.exec();
}

#include "main.moc"