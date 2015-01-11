--鉄の騎士 ギア・フリード
function c423705.initial_effect(c)
	--destroy equip
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(423705,0))
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_EQUIP)
	e1:SetTarget(c423705.destg)
	e1:SetOperation(c423705.desop)
	c:RegisterEffect(e1)
end
function c423705.filter(c,ec)
	return c:GetEquipTarget()==ec
end
function c423705.destg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return eg:IsExists(c423705.filter,1,nil,e:GetHandler()) end
	local dg=eg:Filter(c423705.filter,nil,e:GetHandler())
	Duel.SetTargetCard(dg)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,dg,dg:GetCount(),0,0)
end
function c423705.desop(e,tp,eg,ep,ev,re,r,rp)
	local tg=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS):Filter(Card.IsRelateToEffect,nil,e)
	Duel.Destroy(tg,REASON_EFFECT)
end
