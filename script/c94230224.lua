--ニードル·ボール
function c94230224.initial_effect(c)
	--flip
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(94230224,0))
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_FLIP)
	e1:SetCost(c94230224.cost)
	e1:SetTarget(c94230224.target)
	e1:SetOperation(c94230224.operation)
	c:RegisterEffect(e1)
end
function c94230224.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	if Duel.CheckLPCost(tp,2000) and Duel.SelectYesNo(tp,aux.Stringid(94230224,1)) then
		Duel.PayLPCost(tp,2000)
		e:SetLabel(1)
		e:SetCategory(CATEGORY_DAMAGE)
	else
		e:SetLabel(0)
		e:SetCategory(0)
	end
end
function c94230224.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	if e:GetLabel()==1 then
		Duel.SetTargetPlayer(1-tp)
		Duel.SetTargetParam(1000)
		Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,1000)
	end
end
function c94230224.operation(e,tp,eg,ep,ev,re,r,rp)
	if e:GetLabel()==1 then
		local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
		Duel.Damage(p,d,REASON_EFFECT)
	end
end
