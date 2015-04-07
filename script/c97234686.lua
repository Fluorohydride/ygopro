--グラディアル・チェンジ
function c97234686.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCategory(CATEGORY_HANDES)
	e1:SetCode(EVENT_SPSUMMON_SUCCESS)
	e1:SetCondition(c97234686.condition)
	e1:SetTarget(c97234686.target)
	e1:SetOperation(c97234686.activate)
	c:RegisterEffect(e1)
end
function c97234686.filter(c,tp)
	return c:IsFaceup() and c:IsControler(tp) and c:IsSetCard(0x19)
end
function c97234686.condition(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c97234686.filter,1,nil,tp)
end
function c97234686.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFieldGroupCount(tp,0,LOCATION_HAND)>0 end
	Duel.SetOperationInfo(0,CATEGORY_HANDES,nil,0,tp,1)
end
function c97234686.activate(e,tp,eg,ep,ev,re,r,rp)
	Duel.DiscardHand(1-tp,aux.TRUE,1,1,REASON_DISCARD+REASON_EFFECT)
end
