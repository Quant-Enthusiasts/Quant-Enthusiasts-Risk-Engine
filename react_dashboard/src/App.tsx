import React from "react";
import { BrowserRouter as Router, Routes, Route } from "react-router-dom";
import {
   Dashboard } from "./pages/Dashboard";
import { Documentation } from "./pages/Documentation";

function App() {
  return (
    <Router>
      <Routes>
        <Route path="/" element={<Dashboard />} />
        <Route path="/documentation" element={<Documentation />} />
      </Routes>
    </Router>
  );
}

export default App;
