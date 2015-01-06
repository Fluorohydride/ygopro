--ダーク・ダイブ・ボンバー
function c32646477.initial_effect(c)
	--synchro summon
	aux.AddSynchroProcedure(c,nil,aux.NonTuner(nil),1)
	c:EnableReviveLimit()
	--damage
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(32646477,0))
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetCategory(CATEGORY_DAMAGE)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1,32646477)
	e1:SetCondition(c32646477.condition)
	e1:SetCost(c32646477.cost)
	e1:SetTarget(c32646477.target)
	e1:SetOperation(c32646477.operation)
	c:RegisterEffect(e1)
end
function c32646477.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetCurrentPhase()==PHASE_MAIN1
end
function c32646477.costfilter(c)
	return c:GetLevel()>0
end
function c32646477.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckReleaseGroup(tp,c32646477.costfilter,1,nil) end
	local g=Duel.SelectReleaseGroup(tp,c32646477.costfilter,1,1,nil)
	e:SetLabel(g:GetFirst():GetLevel()*200)
	Duel.Release(g,REASON_COST)
end
function c32646477.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetTargetPlayer(1-tp)
	Duel.SetTargetParam(e:GetLabel())
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,e:GetLabel())
	e:SetLabel(0)
end
function c32646477.operation(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Damage(p,d,REASON_EFFECT)
end
