--ワーム・ファルコ
function c58760121.initial_effect(c)
	--flip
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FLIP+EFFECT_TYPE_SINGLE)
	e1:SetCategory(CATEGORY_POSITION)
	e1:SetTarget(c58760121.postg)
	e1:SetOperation(c58760121.posop)
	c:RegisterEffect(e1)
end
function c58760121.filter(c)
	return c:IsFaceup() and c:IsSetCard(0x3e) and c:IsRace(RACE_REPTILE)
end
function c58760121.postg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local g=Duel.GetMatchingGroup(c58760121.filter,tp,LOCATION_MZONE,0,e:GetHandler())
	Duel.SetOperationInfo(0,CATEGORY_POSITION,g,g:GetCount(),0,0)
end
function c58760121.posop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c58760121.filter,tp,LOCATION_MZONE,0,e:GetHandler())
	Duel.ChangePosition(g,POS_FACEDOWN_DEFENCE,0,POS_FACEDOWN_DEFENCE,0)
end
