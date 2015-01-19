--フレムベル・デビル
function c91711547.initial_effect(c)
	--damage
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(91711547,0))
	e1:SetCategory(CATEGORY_DAMAGE)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetCode(EVENT_BATTLE_DAMAGE)
	e1:SetCondition(c91711547.damcon)
	e1:SetTarget(c91711547.damtg)
	e1:SetOperation(c91711547.damop)
	c:RegisterEffect(e1)
end
function c91711547.damcon(e,tp,eg,ep,ev,re,r,rp)
	return ep~=tp
end
function c91711547.damtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetTargetPlayer(1-tp)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,0)
end
function c91711547.damop(e,tp,eg,ep,ev,re,r,rp)
	local p=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER)
	local d=Duel.GetMatchingGroupCount(Card.IsRace,tp,LOCATION_GRAVE,0,nil,RACE_PYRO)*200
	Duel.Damage(p,d,REASON_EFFECT)
end