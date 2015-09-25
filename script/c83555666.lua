--破壊輪
function c83555666.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY+CATEGORY_DAMAGE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,0x1e0)
	e1:SetCountLimit(1,83555666+EFFECT_COUNT_CODE_OATH)
	e1:SetCondition(c83555666.condition)
	e1:SetTarget(c83555666.target)
	e1:SetOperation(c83555666.activate)
	c:RegisterEffect(e1)
end
function c83555666.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()~=tp
end
function c83555666.filter(c,lp)
	return c:IsFaceup() and c:IsAttackBelow(lp) and c:IsDestructable()
end
function c83555666.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	local lp=Duel.GetLP(1-tp)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(1-tp) and c83555666.filter(chkc,lp) end
	if chk==0 then return Duel.IsExistingTarget(c83555666.filter,tp,0,LOCATION_MZONE,1,nil,lp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,c83555666.filter,tp,0,LOCATION_MZONE,1,1,nil,lp)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,PLAYER_ALL,0)
end
function c83555666.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and tc:IsFaceup() and Duel.Destroy(tc,REASON_EFFECT)~=0 then
		local atk=tc:GetTextAttack()
		if atk<0 then atk=0 end
		local val=Duel.Damage(tp,atk,REASON_EFFECT)
		if val>0 and Duel.GetLP(tp)>0 then
			Duel.BreakEffect()
			Duel.Damage(1-tp,val,REASON_EFFECT)
		end
	end
end
