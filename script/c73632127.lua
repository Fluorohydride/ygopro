--ディメンション・スライド
function c73632127.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_REMOVE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_SPSUMMON_SUCCESS)
	e1:SetCondition(c73632127.condition)
	e1:SetTarget(c73632127.target)
	e1:SetOperation(c73632127.activate)
	c:RegisterEffect(e1)
	--
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_TRAP_ACT_IN_SET_TURN)
	e2:SetProperty(EFFECT_FLAG_SET_AVAILABLE)
	c:RegisterEffect(e2)
end
function c73632127.condition(e,tp,eg,ep,ev,re,r,rp)
	if eg:GetCount()==1 and eg:GetFirst():GetSummonType()==SUMMON_TYPE_XYZ
		and eg:GetFirst():IsControler(tp) then return true end
	if e:GetHandler():IsStatus(STATUS_SET_TURN) then return false end
	return eg:IsExists(Card.IsControler,1,nil,tp)
end
function c73632127.filter(c)
	return c:IsFaceup() and c:IsAbleToRemove()
end
function c73632127.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(1-tp) and c73632127.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c73632127.filter,tp,0,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g=Duel.SelectTarget(tp,c73632127.filter,tp,0,LOCATION_MZONE,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,g,1,0,0)
end
function c73632127.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsFaceup() and tc:IsRelateToEffect(e) then
		Duel.Remove(tc,POS_FACEUP,REASON_EFFECT)
	end
end
