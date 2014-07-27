--黒蛇病
function c47233801.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--damage
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(47233801,0))
	e2:SetCategory(CATEGORY_DAMAGE)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCountLimit(1)
	e2:SetCode(EVENT_PHASE+PHASE_STANDBY)
	e2:SetCondition(c47233801.damcon)
	e2:SetTarget(c47233801.damtg)
	e2:SetOperation(c47233801.damop)
	c:RegisterEffect(e2)
end
function c47233801.damcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()==tp
end
function c47233801.damtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,PLAYER_ALL,0)
end
function c47233801.damop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if not c:IsRelateToEffect(e) then return end
	local dam=c:GetFlagEffectLabel(47233801)
	if dam==nil then
		c:RegisterFlagEffect(47233801,RESET_EVENT+0x1fe0000,0,0,200)
		dam=200
	else
		dam=dam*2
		c:SetFlagEffectLabel(47233801,dam)
	end
	Duel.Damage(tp,dam,REASON_EFFECT)
	Duel.Damage(1-tp,dam,REASON_EFFECT)
end
