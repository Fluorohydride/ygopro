--堕ち影の蠢き
function c77505534.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,0x11e8)
	e1:SetTarget(c77505534.target)
	e1:SetOperation(c77505534.activate)
	c:RegisterEffect(e1)
end
function c77505534.filter(c)
	return c:IsSetCard(0x9d) and c:IsAbleToGrave()
end
function c77505534.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c77505534.filter,tp,LOCATION_DECK,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,nil,1,tp,LOCATION_DECK)
end
function c77505534.posfilter(c)
	return c:IsFacedown() and c:IsSetCard(0x9d)
end
function c77505534.activate(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local g=Duel.SelectMatchingCard(tp,c77505534.filter,tp,LOCATION_DECK,0,1,1,nil)
	if g:GetCount()>0 and Duel.SendtoGrave(g,REASON_EFFECT)~=0 then
		local tg=Duel.GetMatchingGroup(c77505534.posfilter,tp,LOCATION_MZONE,0,nil)
		if tg:GetCount()>0 and Duel.SelectYesNo(tp,aux.Stringid(77505534,0)) then
			Duel.BreakEffect()
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_POSCHANGE)
			local sg=tg:Select(tp,1,5,nil)
			Duel.ChangePosition(sg,POS_FACEUP_DEFENCE)
		end
	end
end
