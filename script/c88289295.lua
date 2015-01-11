--シンクロ・コントロール
function c88289295.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_CONTROL)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,0x1c0)
	e1:SetCondition(c88289295.condition)
	e1:SetCost(c88289295.cost)
	e1:SetTarget(c88289295.target)
	e1:SetOperation(c88289295.activate)
	c:RegisterEffect(e1)
end
function c88289295.condition(e,tp,eg,ep,ev,re,r,rp)
	return not Duel.IsExistingMatchingCard(Card.IsType,tp,LOCATION_MZONE+LOCATION_GRAVE,0,1,nil,TYPE_SYNCHRO)
end
function c88289295.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckLPCost(tp,1000) end
	Duel.PayLPCost(tp,1000)
end
function c88289295.filter(c)
	return c:IsFaceup() and c:IsType(TYPE_SYNCHRO) and c:IsControlerCanBeChanged()
end
function c88289295.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(1-tp) and c88289295.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c88289295.filter,tp,0,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_CONTROL)
	local g=Duel.SelectTarget(tp,c88289295.filter,tp,0,LOCATION_MZONE,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_CONTROL,g,1,0,0)
end
function c88289295.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	local ct=1
	if Duel.GetTurnPlayer()~=tp then ct=2 end
	if tc:IsFaceup() and tc:IsRelateToEffect(e) and not Duel.GetControl(tc,tp,PHASE_END,ct) then
		if not tc:IsImmuneToEffect(e) and tc:IsAbleToChangeControler() then
			Duel.Destroy(tc,REASON_EFFECT)
		end
	end
end
