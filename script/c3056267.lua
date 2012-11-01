--王座の侵略者
function c3056267.initial_effect(c)
	--flip
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(3056267,0))
	e1:SetCategory(CATEGORY_CONTROL)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_FLIP)
	e1:SetCondition(c3056267.condition)
	e1:SetTarget(c3056267.target)
	e1:SetOperation(c3056267.operation)
	c:RegisterEffect(e1)
end
function c3056267.swapfilter(c)
	return c:IsAbleToChangeControler()
end
function c3056267.condition(e)
	local ph=Duel.GetCurrentPhase()
	return not (ph==PHASE_BATTLE or ph==PHASE_DAMAGE or ph==PHASE_DAMAGE_CAL)
end
function c3056267.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return false end
	if chk==0 then return true end
	if Duel.IsExistingTarget(c3056267.swapfilter,tp,0,LOCATION_MZONE,1,nil) then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_CONTROL)
		local mon=Duel.SelectTarget(tp,c3056267.swapfilter,tp,0,LOCATION_MZONE,1,1,nil)
		mon:AddCard(e:GetHandler())
		Duel.SetOperationInfo(0,CATEGORY_CONTROL,mon,2,0,0)
	end
end
function c3056267.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) and tc and tc:IsRelateToEffect(e) then
		Duel.SwapControl(c,tc)
	end
end
