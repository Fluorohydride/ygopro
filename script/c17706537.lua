--マンモ・フォッシル
function c17706537.initial_effect(c)
	--damage
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(17706537,0))
	e1:SetCategory(CATEGORY_DAMAGE)
	e1:SetCode(EVENT_BATTLE_DESTROYING)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCondition(aux.bdogcon)
	e1:SetTarget(c17706537.damtg)
	e1:SetOperation(c17706537.damop)
	c:RegisterEffect(e1)
end
function c17706537.damtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetTargetPlayer(1-tp)
	Duel.SetTargetParam(400)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,400)
end
function c17706537.damop(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Damage(p,d,REASON_EFFECT)
end
