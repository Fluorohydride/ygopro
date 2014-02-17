--地雷蜘蛛
function c94773007.initial_effect(c)
	--coin
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(94773007,0))
	e1:SetCategory(CATEGORY_COIN)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_ATTACK_ANNOUNCE)
	e1:SetTarget(c94773007.attg)
	e1:SetOperation(c94773007.atop)
	c:RegisterEffect(e1)
end
function c94773007.attg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_COIN,nil,0,tp,1)
end
function c94773007.atop(e,tp,eg,ep,ev,re,r,rp)
	local opt=Duel.SelectOption(tp,60,61)
	local coin=Duel.TossCoin(tp,1)
	if opt==coin then
		Duel.SetLP(tp,Duel.GetLP(tp)/2)
	end
end
