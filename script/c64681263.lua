--局地的大ハリケーン
function c64681263.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TODECK)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c64681263.target)
	e1:SetOperation(c64681263.operation)
	c:RegisterEffect(e1)
end
function c64681263.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFieldGroupCount(tp,LOCATION_HAND+LOCATION_GRAVE,0)>0 end
	local g=Duel.GetFieldGroup(tp,LOCATION_HAND+LOCATION_GRAVE,0)
	Duel.SetOperationInfo(0,CATEGORY_TODECK,g,g:GetCount(),0,0)
end
function c64681263.operation(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetFieldGroup(tp,LOCATION_HAND+LOCATION_GRAVE,0)
	Duel.SendtoDeck(g,nil,2,REASON_EFFECT)
end
