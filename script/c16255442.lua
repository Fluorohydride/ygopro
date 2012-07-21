--光の召集
function c16255442.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_HANDES+CATEGORY_TOHAND)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c16255442.target)
	e1:SetOperation(c16255442.operation)
	c:RegisterEffect(e1)
end
function c16255442.filter(c)
	return c:IsAttribute(ATTRIBUTE_LIGHT) and c:IsAbleToHand()
end
function c16255442.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		local hd=Duel.GetFieldGroupCount(tp,LOCATION_HAND,0)
		if e:GetHandler():IsLocation(LOCATION_HAND) then hd=hd-1 end
		return hd>0 and Duel.IsExistingMatchingCard(c16255442.filter,tp,LOCATION_GRAVE,0,hd,nil)
	end
	local sg=Duel.GetFieldGroup(tp,LOCATION_HAND,0)
	local tg=Duel.GetMatchingGroup(c16255442.filter,tp,LOCATION_GRAVE,0,nil)
	Duel.SetOperationInfo(0,CATEGORY_HANDES,sg,sg:GetCount(),0,0)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,tg,sg:GetCount(),0,0)
end
function c16255442.operation(e,tp,eg,ep,ev,re,r,rp)
	local sg=Duel.GetFieldGroup(tp,LOCATION_HAND,0)
	local sct=sg:GetCount()
	Duel.SendtoGrave(sg,REASON_EFFECT+REASON_DISCARD)
	local tg=Duel.GetMatchingGroup(c16255442.filter,tp,LOCATION_GRAVE,0,nil)
	if tg:GetCount()>=sct then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
		local sel=tg:Select(tp,sct,sct,nil)
		Duel.SendtoHand(sel,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,sel)
	end
end
