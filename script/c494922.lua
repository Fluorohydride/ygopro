--超重荒神 スサノ－O
function c494922.initial_effect(c)
	--synchro summon
	aux.AddSynchroProcedure(c,aux.FilterBoolFunction(Card.IsRace,RACE_MACHINE),aux.NonTuner(Card.IsSetCard,0x9a),1)
	c:EnableReviveLimit()
	--
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_DEFENCE_ATTACK)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(LOCATION_MZONE,0)
	e1:SetTarget(c494922.atktg)
	e1:SetValue(1)
	c:RegisterEffect(e1)
	--
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(494922,0))
	e2:SetCategory(CATEGORY_CONTROL)
	e2:SetType(EFFECT_TYPE_QUICK_O)
	e2:SetCode(EVENT_FREE_CHAIN)
	e2:SetRange(LOCATION_MZONE)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetCountLimit(1)
	e2:SetCondition(c494922.condition)
	e2:SetTarget(c494922.target)
	e2:SetOperation(c494922.operation)
	c:RegisterEffect(e2)
end
function c494922.atktg(e,c)
	return c:IsSetCard(0x9a)
end
function c494922.condition(e,tp,eg,ep,ev,re,r,rp)
	return not Duel.IsExistingMatchingCard(Card.IsType,e:GetHandlerPlayer(),LOCATION_GRAVE,0,1,nil,TYPE_SPELL+TYPE_TRAP) 
end
function c494922.filter(c)
	return c:IsType(TYPE_SPELL+TYPE_TRAP) and c:IsSSetable()
end
function c494922.filter2(c)
	return c:IsType(TYPE_FIELD) and c:IsSSetable()
end
function c494922.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControlerCanBeChanged() end
	if chk==0 then return Duel.IsExistingMatchingCard(c494922.filter,tp,0,LOCATION_GRAVE,1,nil) and (Duel.GetLocationCount(tp,LOCATION_SZONE)>0 or Duel.IsExistingMatchingCard(c494922.filter2,tp,0,LOCATION_GRAVE,1,nil)) end
	local g=Group.CreateGroup()
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SET)
	if Duel.GetLocationCount(tp,LOCATION_SZONE)==0 then
		g=Duel.SelectTarget(tp,c494922.filter2,tp,0,LOCATION_GRAVE,1,1,nil)
	else
		g=Duel.SelectTarget(tp,c494922.filter,tp,0,LOCATION_GRAVE,1,1,nil)
	end
	Duel.SetOperationInfo(0,CATEGORY_CONTROL,g,1,0,0)
end
function c494922.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if Duel.GetLocationCount(tp,LOCATION_SZONE)<=0 and not tc:IsType(TYPE_FIELD) then return end
	if tc:IsRelateToEffect(e) then
		Duel.SSet(tp,tc)
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_LEAVE_FIELD_REDIRECT)
		e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		e1:SetValue(LOCATION_REMOVED)
		tc:RegisterEffect(e1,true)
	end
end
