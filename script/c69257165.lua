--精神汚染
function c69257165.initial_effect(c)
	--control
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCategory(CATEGORY_CONTROL)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCost(c69257165.cost)
	e1:SetTarget(c69257165.target)
	e1:SetOperation(c69257165.operation)
	c:RegisterEffect(e1)
end
function c69257165.ctffilter(c,lv)
	return c:IsFaceup() and c:IsControlerCanBeChanged() and c:GetLevel()==lv
end
function c69257165.ctfilter(c,tp)
	return c:IsType(TYPE_MONSTER) and c:IsDiscardable()
		and Duel.IsExistingTarget(c69257165.ctffilter,tp,0,LOCATION_MZONE,1,nil,c:GetLevel())
end
function c69257165.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	e:SetLabel(100)
	return true
end
function c69257165.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(1-tp) and c69257165.ctffilter(chkc,e:GetLabel()) end
	if chk==0 then
		if e:GetLabel()~=100 then return false end
		e:SetLabel(0)
		return Duel.IsExistingMatchingCard(c69257165.ctfilter,tp,LOCATION_HAND,0,1,nil,tp)
	end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DISCARD)
	local sg=Duel.SelectMatchingCard(tp,c69257165.ctfilter,tp,LOCATION_HAND,0,1,1,nil,tp)
	local lv=sg:GetFirst():GetLevel()
	e:SetLabel(lv)
	Duel.SendtoGrave(sg,REASON_COST+REASON_DISCARD)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_CONTROL)
	local g=Duel.SelectTarget(tp,c69257165.ctffilter,tp,0,LOCATION_MZONE,1,1,nil,lv)
	Duel.SetOperationInfo(0,CATEGORY_CONTROL,g,1,0,0)
end
function c69257165.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) and tc:IsFaceup() and tc:GetLevel()==e:GetLabel() and not Duel.GetControl(tc,tp,PHASE_END,1) then
		if not tc:IsImmuneToEffect(e) and tc:IsAbleToChangeControler() then
			Duel.Destroy(tc,REASON_EFFECT)
		end
	end
end
