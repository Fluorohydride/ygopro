--剣闘調教
function c9780364.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_POSITION)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c9780364.condition)
	e1:SetTarget(c9780364.target)
	e1:SetOperation(c9780364.activate)
	c:RegisterEffect(e1)
end
function c9780364.cfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x19)
end
function c9780364.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c9780364.cfilter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil)
end
function c9780364.filter(c,e)
	return c:IsFaceup() and c:IsCanBeEffectTarget(e)
end
function c9780364.filter2(c)
	return c:IsFaceup() and c:IsSetCard(0x19) and c:IsAbleToChangeControler()
end
function c9780364.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(1-tp) and chkc:IsFaceup() end
	if chk==0 then return Duel.IsExistingTarget(Card.IsFaceup,tp,0,LOCATION_MZONE,1,nil) end
	local g=Duel.GetMatchingGroup(c9780364.filter,tp,0,LOCATION_MZONE,nil,e)
	local cg=g:Filter(c9780364.filter2,nil)
	local sel=0
	Duel.Hint(HINT_SELECTMSG,tp,550)
	if cg:GetCount()==0 then
		sel=Duel.SelectOption(tp,aux.Stringid(9780364,0))
	else sel=Duel.SelectOption(tp,aux.Stringid(9780364,0),aux.Stringid(9780364,1)) end
	if sel==0 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_POSCHANGE)
		local sg=g:Select(tp,1,1,nil)
		Duel.SetTargetCard(sg)
		Duel.SetOperationInfo(0,CATEGORY_POSITION,g,1,0,0)
	else
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_CONTROL)
		local sg=cg:Select(tp,1,1,nil)
		Duel.SetTargetCard(sg)
		Duel.SetOperationInfo(0,CATEGORY_CONTROL,g,1,0,0)
	end
	e:SetLabel(sel)
end
function c9780364.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) and tc:IsFaceup() then
		if e:GetLabel()==0 then
			Duel.ChangePosition(tc,POS_FACEUP_DEFENCE,0,POS_FACEUP_ATTACK,0)
		else
			if not Duel.GetControl(tc,tp,PHASE_END,1) and not tc:IsImmuneToEffect(e) and tc:IsAbleToChangeControler() then
				Duel.Destroy(tc,REASON_EFFECT)
			end
		end
	end
end
