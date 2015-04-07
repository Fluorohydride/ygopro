--隠されし機殻
function c4450854.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCountLimit(1,4450854+EFFECT_COUNT_CODE_OATH)
	e1:SetTarget(c4450854.target)
	e1:SetOperation(c4450854.activate)
	c:RegisterEffect(e1)
end
function c4450854.filter(c)
	return c:IsFaceup() and c:IsSetCard(0xaa) and c:IsType(TYPE_PENDULUM) and c:IsAbleToHand()
end
function c4450854.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c4450854.filter,tp,LOCATION_EXTRA,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_EXTRA)
end
function c4450854.activate(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectMatchingCard(tp,c4450854.filter,tp,LOCATION_EXTRA,0,1,3,nil)
	if g:GetCount()>0 then
		Duel.SendtoHand(g,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,g)
	end
end
