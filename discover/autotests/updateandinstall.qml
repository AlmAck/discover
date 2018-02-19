import QtQuick 2.0
import org.kde.discover.app 1.0
import QtTest 1.1

DiscoverTest
{
    function test_openResource() {
        app.openMode("Update");

        {// we start an upate
            var updatePage = appRoot.stack.currentItem;
            compare(typeName(updatePage), "UpdatesPage")
            compare(updatePage.state, "has-updates", "to update")
            var button = findChild(updatePage, "Button")
            verify(button);
            button.clicked();
            compare(updatePage.state, "has-updates", "updating")
        }

        {//we start installing a resource
            app.openApplication("dummy://dummy.1");
            verify(waitForSignal(appRoot.stack, "currentItemChanged"))

            var button = findChild(appRoot.stack.currentItem, "InstallApplicationButton")
            console.log("button", appRoot.stack.currentItem, button)
            verify(button)
            verify(!button.isActive)
            button.click()
        }

        app.openMode("Update");
        {
            var updatePage = appRoot.stack.currentItem;
            compare(typeName(updatePage), "UpdatesPage")
            compare(updatePage.state, "has-updates", "to update")
            var button = findChild(updatePage, "Button")
            verify(!button.isActive)
        }

        while(updatePage.state != "now-uptodate")
            waitForSignal(updatePage, "stateChanged")
    }
}
