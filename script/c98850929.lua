--聖蛇の息吹
function c98850929.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(98850929,0))
	e1:SetCategory(CATEGORY_TOHAND)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c98850929.condition1)
	e1:SetCost(c98850929.cost)
	e1:SetTarget(c98850929.target1)
	e1:SetOperation(c98850929.operation)
	c:RegisterEffect(e1)
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(98850929,1))
	e2:SetCategory(CATEGORY_TOHAND)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetType(EFFECT_TYPE_ACTIVATE)
	e2:SetCode(EVENT_FREE_CHAIN)
	e2:SetCondition(c98850929.condition2)
	e2:SetCost(c98850929.cost)
	e2:SetTarget(c98850929.target2)
	e2:SetOperation(c98850929.operation)
	c:RegisterEffect(e2)
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(98850929,2))
	e3:SetCategory(CATEGORY_TOHAND)
	e3:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e3:SetType(EFFECT_TYPE_ACTIVATE)
	e3:SetCode(EVENT_FREE_CHAIN)
	e3:SetCondition(c98850929.condition3)
	e3:SetCost(c98850929.cost)
	e3:SetTarget(c98850929.target3)
	e3:SetOperation(c98850929.operation)
	c:RegisterEffect(e3)
end
function c98850929.cfilter(c)
	return c:IsFaceup() and c:IsType(TYPE_FUSION+TYPE_RITUAL+TYPE_SYNCHRO+TYPE_XYZ)
end
function c98850929.typecast(c)
	return bit.band(c:GetType(),TYPE_FUSION+TYPE_RITUAL+TYPE_SYNCHRO+TYPE_XYZ)
end
function c98850929.condition1(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c98850929.cfilter,tp,LOCATION_MZONE,0,nil)
	return g:GetClassCount(c98850929.typecast)>=2
end
function c98850929.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFlagEffect(tp,98850929)==0 end
	Duel.RegisterFlagEffect(tp,98850929,RESET_PHASE+PHASE_END,EFFECT_FLAG_OATH,1)
	Duel.Hint(HINT_OPSELECTED,1-tp,e:GetDescription())
end
function c98850929.filter1(c)
	return c:IsFaceup() and c:IsType(TYPE_MONSTER) and c:IsAbleToHand()
end
function c98850929.target1(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE+LOCATION_REMOVED) and chkc:IsControler(tp) and c98850929.filter1(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c98850929.filter1,tp,LOCATION_GRAVE+LOCATION_REMOVED,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectTarget(tp,c98850929.filter1,tp,LOCATION_GRAVE+LOCATION_REMOVED,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,g:GetCount(),0,0)
end
function c98850929.condition2(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c98850929.cfilter,tp,LOCATION_MZONE,0,nil)
	return g:GetClassCount(c98850929.typecast)>=3
end
function c98850929.filter2(c)
	return c:IsType(TYPE_TRAP) and c:IsAbleToHand()
end
function c98850929.target2(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c98850929.filter2(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c98850929.filter2,tp,LOCATION_GRAVE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectTarget(tp,c98850929.filter2,tp,LOCATION_GRAVE,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,g:GetCount(),0,0)
end
function c98850929.condition3(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c98850929.cfilter,tp,LOCATION_MZONE,0,nil)
	return g:GetClassCount(c98850929.typecast)>=4
end
function c98850929.filter3(c)
	return c:IsType(TYPE_SPELL) and c:GetCode()~=98850929 and c:IsAbleToHand()
end
function c98850929.target3(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c98850929.filter3(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c98850929.filter3,tp,LOCATION_GRAVE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectTarget(tp,c98850929.filter3,tp,LOCATION_GRAVE,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,g:GetCount(),0,0)
end
function c98850929.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) then
		Duel.SendtoHand(tc,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,tc)
	end
end
