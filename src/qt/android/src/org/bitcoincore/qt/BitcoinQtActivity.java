package org.viceversachaincore.qt;

import android.os.Bundle;
import android.system.ErrnoException;
import android.system.Os;

import org.qtproject.qt5.android.bindings.QtActivity;

import java.io.File;

public class ViceversachainQtActivity extends QtActivity
{
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        final File viceversachainDir = new File(getFilesDir().getAbsolutePath() + "/.viceversachain");
        if (!viceversachainDir.exists()) {
            viceversachainDir.mkdir();
        }

        super.onCreate(savedInstanceState);
    }
}
