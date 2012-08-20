--零式魔導粉砕機
function c21237481.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c21237481.target1)
	e1:SetOperation(c21237481.operation)
	c:RegisterEffect(e1)
	--damage
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(21237481,0))
	e2:SetCategory(CATEGORY_DAMAGE)
	e2:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e2:SetType(EFFECT_TYPE_QUICK_O)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EVENT_FREE_CHAIN)
	e2:SetHintTiming(0,0x1e0)
	e2:SetCost(c21237481.cost2)
	e2:SetTarget(c21237481.target2)
	e2:SetOperation(c21237481.operation)
	c:RegisterEffect(e2)
end
function c21237481.costfilter(c)
	return c:IsType(TYPE_SPELL) and c:IsDiscardable()
end
function c21237481.target1(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	if Duel.IsExistingMatchingCard(c21237481.costfilter,tp,LOCATION_HAND,0,1,e:GetHandler()) and
		Duel.SelectYesNo(tp,aux.Stringid(21237481,1)) then
		e:SetCategory(CATEGORY_DAMAGE)
		e:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DISCARD)
		local cg=Duel.SelectMatchingCard(tp,c21237481.costfilter,tp,LOCATION_HAND,0,1,60,nil)
		Duel.SendtoGrave(cg,REASON_COST+REASON_DISCARD)
		local dam=cg:GetCount()*500
		Duel.SetTargetPlayer(1-tp)
		Duel.SetTargetParam(dam)
		Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,dam)
		e:SetLabel(dam)
	else
		e:SetCategory(0)
		e:SetProperty(0)
		e:SetLabel(0)
	end
end
function c21237481.cost2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c21237481.costfilter,tp,LOCATION_HAND,0,1,e:GetHandler()) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DISCARD)
	local cg=Duel.SelectMatchingCard(tp,c21237481.costfilter,tp,LOCATION_HAND,0,1,60,nil)
	Duel.SendtoGrave(cg,REASON_COST+REASON_DISCARD)
	e:SetLabel(cg:GetCount())
end
function c21237481.target2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local dam=e:GetLabel()*500
	Duel.SetTargetPlayer(1-tp)
	Duel.SetTargetParam(dam)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,dam)
end
function c21237481.operation(e,tp,eg,ep,ev,re,r,rp)
	if e:GetLabel()==0 or not e:GetHandler():IsRelateToEffect(e) then return end
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Damage(p,d,REASON_EFFECT)
end
