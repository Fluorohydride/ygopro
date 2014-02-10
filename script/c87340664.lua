--原子ホタル
function c87340664.initial_effect(c)
	--damage
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(87340664,0))
	e1:SetCategory(CATEGORY_DAMAGE)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetCode(EVENT_BATTLE_DESTROYED)
	e1:SetCondition(c87340664.condition)
	e1:SetTarget(c87340664.target)
	e1:SetOperation(c87340664.operation)
	c:RegisterEffect(e1)
end
function c87340664.condition(e,tp,eg,ep,ev,re,r,rp)
	e:SetLabel(e:GetHandler():GetReasonPlayer())
	return e:GetHandler():IsReason(REASON_BATTLE) and e:GetHandler():IsPreviousPosition(POS_FACEUP)
end
function c87340664.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chk==0 then return true end
	Duel.SetTargetPlayer(e:GetLabel())
	Duel.SetTargetParam(1000)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,e:GetLabel(),1000)
end
function c87340664.operation(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Damage(p,d,REASON_EFFECT)
end
