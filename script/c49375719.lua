--サイバー・チュチュ
function c49375719.initial_effect(c)
	--direct attack
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_DIRECT_ATTACK)
	e1:SetCondition(c49375719.dacon)
	c:RegisterEffect(e1)
end
function c49375719.filter(c,atk)
	return c:IsFacedown() or c:GetAttack()<=atk
end
function c49375719.dacon(e)
	return not Duel.IsExistingMatchingCard(c49375719.filter,e:GetHandlerPlayer(),0,LOCATION_MZONE,1,nil,e:GetHandler():GetAttack())
end
