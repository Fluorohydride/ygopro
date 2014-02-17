--魔霧雨
function c27827272.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c27827272.cost)
	e1:SetTarget(c27827272.target)
	e1:SetOperation(c27827272.activate)
	c:RegisterEffect(e1)
end
function c27827272.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetCurrentPhase()==PHASE_MAIN1 end
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_CANNOT_BP)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET+EFFECT_FLAG_OATH)
	e1:SetTargetRange(1,0)
	e1:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e1,tp)
end
function c27827272.filter(c,tp)
	return c:IsFaceup() and (c:IsCode(70781052) or c:IsRace(RACE_THUNDER))
		and Duel.IsExistingMatchingCard(c27827272.filter2,tp,0,LOCATION_MZONE,1,nil,c:GetAttack())
end
function c27827272.filter2(c,atk)
	return c:IsFaceup() and c:IsDefenceBelow(atk) and c:IsDestructable()
end
function c27827272.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(tp) and c27827272.filter(chkc,tp) end
	if chk==0 then return Duel.IsExistingTarget(c27827272.filter,tp,LOCATION_MZONE,0,1,nil,tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TARGET)
	local g=Duel.SelectTarget(tp,c27827272.filter,tp,LOCATION_MZONE,0,1,1,nil,tp)
	local dg=Duel.GetMatchingGroup(c27827272.filter2,tp,0,LOCATION_MZONE,nil,g:GetFirst():GetAttack())
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,dg,dg:GetCount(),0,0)
end
function c27827272.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsFaceup() and tc:IsRelateToEffect(e) then
		local dg=Duel.GetMatchingGroup(c27827272.filter2,tp,0,LOCATION_MZONE,nil,tc:GetAttack())
		Duel.Destroy(dg,REASON_EFFECT)
	end
end
