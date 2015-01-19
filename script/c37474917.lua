--カタパルト・ウォリアー
function c37474917.initial_effect(c)
	--synchro summon
	aux.AddSynchroProcedure(c,nil,aux.NonTuner(nil),1)
	c:EnableReviveLimit()
	--damage
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(37474917,0))
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetCategory(CATEGORY_DAMAGE)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetCost(c37474917.cost)
	e1:SetTarget(c37474917.target)
	e1:SetOperation(c37474917.operation)
	c:RegisterEffect(e1)
end
function c37474917.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckReleaseGroup(tp,Card.IsSetCard,1,nil,0x43) end
	local sg=Duel.SelectReleaseGroup(tp,Card.IsSetCard,1,1,nil,0x43)
	local atk=sg:GetFirst():GetTextAttack()
	if atk<0 then atk=0 end
	e:SetLabel(atk)
	Duel.Release(sg,REASON_COST)
end
function c37474917.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetTargetPlayer(1-tp)
	Duel.SetTargetParam(e:GetLabel())
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,e:GetLabel())
end
function c37474917.operation(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Damage(p,d,REASON_EFFECT)
end
