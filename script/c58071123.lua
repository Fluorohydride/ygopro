--オキシゲドン
function c58071123.initial_effect(c)
	--damage
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(58071123,0))
	e1:SetCategory(CATEGORY_DAMAGE)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_BATTLE_DESTROYED)
	e1:SetCondition(c58071123.damcon)
	e1:SetTarget(c58071123.damtg)
	e1:SetOperation(c58071123.damop)
	c:RegisterEffect(e1)
end
function c58071123.damcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():IsLocation(LOCATION_GRAVE) and e:GetHandler():GetBattleTarget():IsRace(RACE_PYRO)
end
function c58071123.damtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,PLAYER_ALL,800)
end
function c58071123.damop(e,tp,eg,ep,ev,re,r,rp)
	Duel.Damage(tp,800,REASON_EFFECT)
	Duel.Damage(1-tp,800,REASON_EFFECT)
end
