--D－タイム
function c99075257.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCategory(CATEGORY_TOHAND+CATEGORY_SEARCH)
	e1:SetCode(EVENT_LEAVE_FIELD)
	e1:SetCondition(c99075257.condition)
	e1:SetTarget(c99075257.target)
	e1:SetOperation(c99075257.operation)
	c:RegisterEffect(e1)
end
function c99075257.condition(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	local lv=tc:GetLevel()
	return eg:GetCount()==1 and tc:IsSetCard(0x3008) and tc:IsPreviousPosition(POS_FACEUP) and tc:GetPreviousControler()==tp
end
function c99075257.filter(c,lv)
	return c:IsLevelBelow(lv) and c:IsSetCard(0xc008) and c:IsAbleToHand()
end
function c99075257.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c99075257.filter,tp,LOCATION_DECK,0,1,nil,eg:GetFirst():GetLevel()) end
	e:SetLabel(eg:GetFirst():GetLevel())
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
end
function c99075257.operation(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectMatchingCard(tp,c99075257.filter,tp,LOCATION_DECK,0,1,1,nil,e:GetLabel())
	if g:GetCount()~=0 then
		Duel.SendtoHand(g,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,g)
	end
end
